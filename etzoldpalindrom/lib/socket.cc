#include "socket.hh"
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <openssl/err.h>

socket::socket() : _fd( -1 ), _ctx( NULL ), _ssl( NULL ) {
	_sslclient = true;
}

socket::socket( int fd ) : _fd( fd ), _ctx( NULL ), _ssl( NULL ) {
	_sslclient = true;
}

void socket::close() {
	if( _fd != -1 ) {
		::close( _fd );
	}
}

socket::~socket() {
	close();
	if( _ssl ) {
		SSL_free( _ssl );
	}
	if( _ctx ) {
		SSL_CTX_free( _ctx );
	}
}

std::string socket::get_ip() const {
	return get_ip( _fd );
}

std::string socket::get_ip( int fd ) {
	std::string ret;
	if( fd != -1 ) {
		struct sockaddr addr;
		socklen_t l = sizeof( addr );
		if( ! getpeername( fd, ( struct sockaddr* ) & addr, &l ) ) {
			ret = inet_ntoa( ( ( struct sockaddr_in* ) &addr )->sin_addr );
		}
	}
	return ret;
}

void socket::set_fd( int fd ) {
	_fd = fd;
	if( _ssl ) {
		SSL_free( _ssl );
	}
	_ssl = NULL;
}

bool socket::ssl_init( bool client ) {
	_sslclient = client;
	SSL_library_init();
	SSL_METHOD* m = ( client ? SSLv2_client_method() : SSLv2_server_method() );
	if( ! m ) {
		return false;
	}
	if( ! ( _ctx = SSL_CTX_new( m ) ) ) {
		return false;
	}
	if( ! client ) {
		SSL_CTX_use_certificate_file( _ctx, "cacert.pem", SSL_FILETYPE_PEM );
		SSL_CTX_use_PrivateKey_file( _ctx, "cakey.pem", SSL_FILETYPE_PEM );
		if( ! SSL_CTX_check_private_key( _ctx ) ) {
			return false;
		}
	}
}

bool socket::ssl() {
	if( ! ( _ssl = SSL_new( _ctx ) ) ) {
		return false;
	}
	SSL_clear( _ssl );
	SSL_set_fd( _ssl, _fd );
	if( _sslclient ) {
		if( SSL_connect( _ssl ) == 1 ) {
			return true;
		}
	} else {
		if( SSL_accept( _ssl ) == 1 ) {
			return true;
		}
	}
	SSL_free( _ssl ); _ssl = NULL;
	return false;
}

bool socket::read_line( std::string& s, unsigned int maxlen, int timeout ) const {
	struct timeval tv;
	fd_set readfds;
	bool seencr = false;
	char b;
	s = "";
	while( s.size() < maxlen ) {
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		FD_ZERO( &readfds );
		FD_SET( _fd, &readfds );
		select( _fd + 1, &readfds, NULL, NULL, &tv );
		if( FD_ISSET( _fd, &readfds ) ) {
			ssize_t c = 0;
			if( _ssl ) {
				c = SSL_read( _ssl, (void*) &b, 1 );
			} else {
				c = read( _fd, (void*) &b, 1 );
			}
			if( c == 0 ) {         // end of file
				return true;
			} else if( c == -1 ) { // error
				return false;
			} else {               // got a character
				if( b == '\n' ) {
					return true;
				} else if( b == '\r' ) {
					if( seencr ) {
						s += '\r';
					}
					seencr = true;
				} else {
					if( seencr ) {
						s += '\r';
					}
					s += b;
					seencr = false;
				}
			}
		} else {  // timeout
			return false;
		}
	}
	return true;
}

bool socket::send_line( const std::string& s ) const {
	unsigned int bytessend = 0;
	unsigned int tosend = s.size();
	while( bytessend < s.size() ) {
		int r = 0;
		if( _ssl ) {
			r = SSL_write( _ssl, s.c_str() + bytessend, tosend );
		} else {
			r = send( _fd, s.c_str() + bytessend, tosend, MSG_NOSIGNAL );
		}
		if( r < 0 ) {  // error
			return false;
		}
		bytessend += r;
		tosend -= r;
	}
	if( _ssl ) {
		if( SSL_write( _ssl, "\n", 1 ) < 0 ) {
			return false;
		}
	} else if( send( _fd, "\n", 1, MSG_NOSIGNAL ) == -1 ) {
		return false;
	}
	return true;
}

bool socket::connect_socket( const std::string& srv, int port, bool resolvehost ) {
	_fd = -1;
	const char* ip;
	if( resolvehost ) {
		struct hostent* h = gethostbyname( srv.c_str() );
		if( h == NULL ) {
			return false;
		}
		ip = inet_ntoa( *( (struct in_addr *) h->h_addr ) );
	} else {
		ip = srv.c_str();
	}
	struct sockaddr_in dest_addr;
	if( ( _fd = ::socket( AF_INET, SOCK_STREAM, 0 ) ) != -1 ) {
		memset( &dest_addr, 0, sizeof( dest_addr ) );
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons( port );
		dest_addr.sin_addr.s_addr = inet_addr( ip );
		if( connect( _fd, (struct sockaddr *)&dest_addr, sizeof( struct sockaddr ) ) == -1 ) {
			close();
			_fd = -1;
			return false;
		}
	}
	return true;
}

int init_socket( int port, int queue ) {
	int sock;
	struct sockaddr_in my_addr;
	if ( ( sock = socket( PF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
		perror( "could not create socket" );
		exit( 1 );
	}
	int yes = 1;
	setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) );
	struct linger l = { 0, 0 };
	setsockopt( sock, SOL_SOCKET, SO_LINGER, (void*) &l, sizeof( linger ) );
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons( port );
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset( & ( my_addr.sin_zero ), '\0', 8 );
	if ( bind( sock, (struct sockaddr *) &my_addr, sizeof(struct sockaddr) ) == -1 ) {
		perror( "could not bind to socket" );
		exit( 1 );
	}
	if ( listen( sock, queue ) != 0 ) {
		perror( "could not listen" );
		exit( 1 );
	}
	return sock;
}

int accept_socket( int sock ) {
	socklen_t sin_size = sizeof(struct sockaddr_in);
	struct sockaddr_in their_addr;
	return ::accept( sock, (struct sockaddr *)&their_addr, &sin_size );
}

