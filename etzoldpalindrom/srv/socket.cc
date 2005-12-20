#include "socket.hh"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <cstdio>

std::string get_ip( int fd ) {
	struct sockaddr addr;
	socklen_t l = sizeof( addr );
	std::string ret;
	if( ! getpeername( fd, ( struct sockaddr* ) & addr, &l ) ) {
		ret = inet_ntoa( ( ( struct sockaddr_in* ) &addr )->sin_addr );
	}
	return ret;
}

bool read_line( int fd, std::string& s, unsigned int maxlen, int timeout ) {
	struct timeval tv;
	fd_set readfds;
	bool seencr = false;
	char b;
	s = "";
	while( s.size() < maxlen ) {
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		FD_ZERO( &readfds );
		FD_SET( fd, &readfds );
		select( fd + 1, &readfds, NULL, NULL, &tv );
		if( FD_ISSET( fd, &readfds ) ) {
			ssize_t c = read( fd, (void*) &b, 1 );
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


void send_line( int fd, const std::string& s ) {
	unsigned int bytessend = 0;
	unsigned int tosend = s.size();
	while( bytessend < s.size() ) {
		int r = send( fd, s.c_str() + bytessend, tosend, MSG_NOSIGNAL );
		if( r == -1 ) {  // error
			return;
		}
		bytessend += r;
		tosend -= r;
	}
	send( fd, "\n", 1, MSG_NOSIGNAL );
}
