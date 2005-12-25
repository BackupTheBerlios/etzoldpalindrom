#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

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

	_data_path += "/";
	srand( time( NULL ) );
	int sock = init_socket( SRV_PORT );

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

int get_next_fd( int sock ) {
	int fd;
	while( true ) {
		assert( ( fd = accept_socket( sock ) ) != -1 );
		if( check_ip( socket::get_ip( fd ) ) ) {
			break;
		}
		close( fd );
	}
	return fd;
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

void loop( int len, int soc ) {
	mpz_t x;
	mpz_init( x );
	_log.notice( "listening on port [%d] ...", SRV_PORT );
	socket sock;
	sock.ssl_init( false );
	for( ; ; len = INC_LENGTH( len ) ) {
		_log.notice( "len = [%d]", len );
		number_generator gen( len );
		gen.first_number( x );

		char* tmp = new char[ len + 128 ];
		do {
			std::string s, ip;
			sock.set_fd( get_next_fd( soc ) );
			if( sock.ssl() ) {
				sock.read_line( s, 1024, 10 );
				ip = sock.get_ip();
				_log.notice( "rcv: [%s] on [%d], line [%s]", ip.c_str(), time( NULL ), s.c_str() );
				std::vector< std::string > v;
				split( s, v );
				if( ! v.empty() && v[ 0 ].find_first_not_of( "0123456789" ) == std::string::npos ) {
					switch( atoi( v[ 0 ].c_str() ) ) {
						case CMD_NICK:
							if( v.size() == 2 ) {
								send_answer( sock, account_check_nick( v[ 1 ] ) );
							} else {
								send_answer( sock, RET_ERR );
							}
							break;
						case CMD_REGISTER:
							if( v.size() == 5 ) {
								send_answer( sock, account_register( v[ 1 ], v[ 2 ], v[ 3 ], v[ 4 ] ) );
							} else {
								send_answer( sock, RET_ERR );
							}
							break;
						case CMD_GET_CONFIG:
							if( v.size() == 3 ) {
								std::string realname, pub;
								int r = account_get_config( v[ 1 ], v[ 2 ], realname, pub );
								send_answer( sock, r, 2, realname.c_str(), pub.c_str() );
							} else {
								send_answer( sock, RET_ERR );
							}
					}
				}
				if( ! s.empty() ) {
					switch( s[ 0 ] ) {
						case 'F':      // client found an etzold palindrom
							sock.close();
							break;
						case 'G':
							mpz_get_str( tmp, 10, x );
							compact( tmp, s );
							std::stringstream out;
							out << ip << " " << time( NULL ) << " 1.0 " << NUMBERS << " " << len << " [" << s << "] " << get_id();
							u_int32_t chk = chksum( out.str() );
							out << " " << chk;
							_log.notice( "snd: [%s]", out.str().c_str() );
							sock.send_line( out.str() );
					}
				}
			} else {
				_log.error( "ssl failed [%s] on [%d]", ip.c_str(), time( NULL ) );
			}
			sock.close();
			_log.debug( "close connection [%s] on [%d]", ip.c_str(), time( NULL ) );
		} while( ! gen.next_number( x, NUMBERS ) );
		delete[] tmp;
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

