#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "socket.hh"
#include "number_generator.hh"
#include "args.hh"
#include "libpal.hh"
#include "logging.hh"
#include "settings.hh"
#include "accounts.hh"

#define INC_LENGTH( x ) ( x * 2 + 1 )

logging     _log( SRV_LOG_FILE, logging::_debug );
std::string _data_path = SRV_DATA_PATH;

std::string get_id( int len = 10 );
bool        check_ip( const std::string& ip );
void        loop( int len, int sock );

int main( int argc, char** argv ) {
	int len = START_LEN;
		
	if( arg_exists( argc, argv, "--len" ) ) {
		len = atoi( arg_value( argc, argv, "--len" ) );
	}
	if( arg_exists( argc, argv, "--datapath" ) ) {
		_data_path = arg_value( argc, argv, "--datapath" );
	}
	if( arg_exists( argc, argv, "--logpath" ) ) {
		_log.set_path( arg_value( argc, argv, "--logpath" ) );
	}

	_data_path += "/";
	srand( time( NULL ) );
	int sock = init_socket( SRV_PORT );

	signal( SIGPIPE, SIG_IGN );

	if( ! arg_exists( argc, argv, "-f" ) ) {
		pid_t p = fork();
		if( p > 0 ) {
			_log.notice( "switched into background, child pid [%d]", p );
			return 0;
		} else if( p == -1 ) {
			std::cerr << "Could not switch into background. Starting in foreground mode." << std::endl;
		}
	}

	loop( len, sock );
}

void send_answer( const socket& sock, int ret ) {
	std::stringstream s;
	s << ret;
	sock.send_line( s.str() );
}

void send_answer( const socket& sock, int ret, int n, ... ) {
	std::stringstream s;
	va_list ap;
	va_start( ap, n );
	s << ret;
	for( int i = 0; i < n; ++i ) {
		s << " [" << va_arg( ap, char* ) << "]";
	}
	va_end( ap );
	sock.send_line( s.str() );
}

int get_next_fd( int sock ) {
	int fd;
	while( true ) {
		assert( ( fd = accept_socket( sock ) ) != -1 );
		if( check_ip( socket::get_ip( fd ) ) ) {
			break;
		}
		socket s( fd );
		if( s.ssl_init( false ) && s.ssl() ) {
			send_answer( s, RET_TOO_MANY_REQUESTS );
		}
	}
	return fd;
}

bool check_command( const socket& sock, const std::vector< std::string >& v, mpz_t x, char* tmp ) {
	if( ! ( v.size() > 1 && v[ 0 ].find_first_not_of( "0123456789." ) == std::string::npos &&
			v[ 1 ].find_first_not_of( "0123456789" ) == std::string::npos ) ) {
		return false;
	}
	if( v[ 0 ] != "0.0.1" ) {
		send_answer( sock, RET_INVALID_VERSION );
	}
	switch( atoi( v[ 1 ].c_str() ) ) {
		case CMD_NICK:
			if( v.size() == 3 ) {
				send_answer( sock, account_check_nick( v[ 2 ] ) );
				return false;
			}
		case CMD_REGISTER:
			if( v.size() == 6 ) {
				send_answer( sock, account_register( v[ 2 ], v[ 3 ], v[ 4 ], v[ 5 ] ) );
				return false;
			}
		case CMD_GET_CONFIG:
			if( v.size() == 4 ) {
				std::string realname, pub;
				int r = account_get_config( v[ 2 ], v[ 3 ], realname, pub );
				send_answer( sock, r, 2, realname.c_str(), pub.c_str() );
				return false;
			}
		case CMD_PALINDROM:
			if( v.size() == 5 ) {
				_log.notice( "got palindrom: [%s], nick [%s], id [%s]", v[ 3 ].c_str(),
					v[ 2 ].c_str(), v[ 4 ].c_str() );
				send_answer( sock, RET_OK );
				return true;
			}
		case CMD_GET_JOB:
			std::string id = get_id();
			std::string s;
			std::stringstream n;
			n << NUMBERS;
			mpz_get_str( tmp, 10, x );
			compact( tmp, s );
			send_answer( sock, RET_OK, 3, s.c_str(), n.str().c_str(), id.c_str() );
			_log.notice( "snd: [%s], nick [%s], id [%s]", s.c_str(), v[ 2 ].c_str(), id.c_str() );
			return false;
	}
	send_answer( sock, RET_ERR );
	return false;
}

void store_number( mpz_t x, char* tmp ) {
	mpz_get_str( tmp, 10, x );
	std::ofstream f( ( _data_path + "lastnumber" ).c_str(), std::ios::trunc );
	if( ! f.is_open() || f.fail() ) {
		_log.error( "could not open file lastnumber for writing" );
		return;
	}
	std::string s;
	compact( tmp, s );
	f << s;
}

std::string read_number() {
	std::string s;
	std::ifstream f( ( _data_path + "lastnumber" ).c_str() );
	if( ! f.is_open() || f.fail() ) {
		_log.notice( "could not open file lastnumber for reading" );
		return s;
	}
	std::getline( f, s );
	_log.notice( "continue with number [%s]", s.c_str() );
	decompact( s, s );
	return s;
}

void loop( int len, int soc ) {
	std::string s;
	number_generator* gen = NULL;
	socket sock;
	mpz_t x;

	_log.notice( "listening on port [%d] ...", SRV_PORT );
	mpz_init( x );
	sock.ssl_init( false );
	s = read_number();
	if( ! s.empty() ) {
		gen = new number_generator( s );
		mpz_set_str( x, s.c_str(), 10 );
		len = s.size();
	}
	for( ; ; len = INC_LENGTH( len ) ) {
		_log.notice( "len = [%d]", len );
		if( ! gen ) {
			gen = new number_generator( len );
			gen->first_number( x );
		}
		char* tmp = new char[ len + 128 ];
		do {
			store_number( x, tmp );
			std::string ip;
			sock.set_fd( get_next_fd( soc ) );
			if( sock.ssl() ) {
				sock.read_line( s, 1024, 10 );
				ip = sock.get_ip();
				_log.notice( "rcv: [%s] line [%s]", ip.c_str(), s.c_str() );
				std::vector< std::string > v;
				split( s, v );
				if( check_command( sock, v, x, tmp ) ) {
					break;
				}
			} else {
				_log.error( "ssl failed [%s] on [%d]", ip.c_str(), time( NULL ) );
			}
			sock.close();
			_log.debug( "close connection [%s]", ip.c_str() );
		} while( ! gen->next_number( x, NUMBERS ) );
		delete[] tmp;
		delete gen;
		gen = NULL;
	}
}

std::string get_id( int len ) {
	static const char* sigma = 
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string s;
	for( int l = strlen( sigma ); len; --len ) {
		s += sigma[ rand() % l ];
	}
	return s;
}

bool check_ip( const std::string& ip ) {
	bool ret = true;
	static std::list< std::pair< std::string, time_t > > cache;
	static std::map< std::string, int > ipcnt;
	std::map< std::string, int >::iterator it;
	time_t t = time( NULL );

	cache.push_back( std::make_pair( ip, t ) );
	if( ( it = ipcnt.find( ip ) ) != ipcnt.end() ) {
		if( it->second++ >= MAX_REQUESTS_PER_MINUTE ) {
			ret = false;
		}
	} else {
		ipcnt[ ip ] = 1;
	}
	while( ! cache.empty() && t - cache.front().second > 60 ) {
		if( ( it = ipcnt.find( ip ) ) != ipcnt.end() ) {
			if( it->second-- == 1 ) {
				ipcnt.erase( it );
			}
		}
		cache.pop_front();
	}
	_log.debug( "cache size [%d], unique ips [%d]", cache.size(), ipcnt.size() );
	return ret;
}

