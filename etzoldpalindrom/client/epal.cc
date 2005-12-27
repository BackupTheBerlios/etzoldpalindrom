#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <openssl/ssl.h>

#include "libpal.hh"
#include "settings.hh"
#include "socket.hh"
#include "number_generator.hh"
#include "args.hh"
#include "srv.hh"
#include "usr.hh"
#include "types.hh"

std::string _version = CLIENT_VERSION;
std::string _srv = SRV_ADDR;
bool        _ip = false;
int         _port = SRV_PORT;

void read_config( config_t& c );
void write_config( const config_t& c );
void loop( const config_t& c );
std::string check( const std::string& num, int n );

int main( int argc, char** argv ) {
	config_t conf;

	if( arg_exists( argc, argv, "--hostip" ) ) {
		_srv = arg_value( argc, argv, "--hostip" );
		_ip = true;
	}
	if( arg_exists( argc, argv, "--port" ) ) {
		_port = atoi( arg_value( argc, argv, "--port" ) );
	}

	std::cout << "epal " << _version << std::endl;
	read_config( conf );
	std::cout << "using nick " << conf.nick << std::endl;

	signal( SIGPIPE, SIG_IGN );

	if( ! arg_exists( argc, argv, "-f" ) ) {  // daemonize
		pid_t p = fork();
		if( p > 0 ) {
			std::cout << "child process " << p << std::endl;
			exit( 0 );
		} else if( p == -1 ) {
			std::cerr << "Could not switch into background." << std::endl;
			exit( 1 );
		}
	}

	loop( conf );
}

void write_config( config_t& c ) {
	std::ofstream f( CONFIG_FILE, std::ios::trunc );
	if( ! f.is_open() || f.fail() ) {
		std::cerr << "Could not open configuration file " << CONFIG_FILE << " for writing." << std::endl;
		exit( 1 );
	}
	f << "nick: " << c.nick << std::endl;
	f << "pwd: " << c.pwd << std::endl;
	f << "public: " << c.pub << std::endl;
	f << "realname: " << c.realname << std::endl;
}

void read_config( config_t& c ) {
	std::ifstream f( CONFIG_FILE );
	std::string s;
	if( ! f.is_open() || f.fail() ) {
		std::cout << "No configuration file was found." << std::endl;
		do {
			std::cout << "Do you already have an account? [N/y] " << std::flush;
			std::getline( std::cin, s );
		} while( s != "n" && s != "y" && s != "N" && s != "Y" && ! s.empty() );
		if( s == "y" || s == "Y" ) {
			use_account( c );
		} else {
			register_account( c );
		}
		write_config( c );
	} else {
		while( std::getline( f, s ) ) {
			std::string::size_type p = s.find( ": " );
			if( p != std::string::npos ) {
				std::string key = s.substr( 0, p );
				std::string val = s.substr( p );
				val.erase( 0, 2 );
				if( key == "nick" ) {
					c.nick = val;
				} else if( key == "pwd" ) {
					c.pwd = val;
				} else if( key == "public" ) {
					c.pub = val;
				} else if( key == "realname" ) {
					c.realname = val;
				}
			}
		}
	}
}

std::string check( const std::string& number, int n ) {
	std::string r;
	std::string num;
	char* tmp;
	mpz_t c, p;
	
	decompact( number, num );
	mpz_init_set_str( p, num.c_str(), 10 );
	mpz_init( c );

	number_generator gen( num );
	mpz_init_set_str( p, num.c_str(), 10 );

	tmp = new char[ 2 * num.size() + 128 ];
	for( ; n; --n ) {
		mpz_mul( c, p, p );
		mpz_get_str( tmp, 10, c );
		if( is_palindrom( tmp ) && mpz_probab_prime_p( p, 10 ) > 0 ) {
			mpz_get_str( tmp, 10, p );
			r = tmp;
			break;
		}
		if( gen.next_number( p ) ) break;
	}
	delete[] tmp;
	mpz_clear( c );
	mpz_clear( p );
	return r;
}

/*
CMD_GET_JOB [nick]
 - ret [startnum] [n] [id]

CMD_PALINDROM [nick] [pal] [id]
 - ret
*/

void loop( const config_t& c ) {
	while( 1 ) {
		std::vector< std::string > v;
		int r;
		if( ( r = send_cmd( CMD_GET_JOB, v, 1, c.nick.c_str() ) ) == RET_OK ) {
			std::string p = check( v[ 0 ], atoi( v[ 1 ].c_str() ) );
			if( ! p.empty() ) { // found palindrom
				compact( p, p );
				std::cout << "found palindrom [" << p << "]^2" << std::endl;
				for( int i = 0; i < 30 && send_cmd( CMD_PALINDROM, 3, c.nick.c_str(), 
						p.c_str(), v[ 2 ].c_str() ) != RET_OK; ++i ) {
					std::cerr << "Could not sent palindrom. Waiting some seconds ..." << std::endl;
					sleep( SRV_RETRY );
				}
			}
		} else if( r == RET_TOO_MANY_REQUESTS ) {
			sleep( SRV_RETRY );
		} else {
			std::cerr << "Could not talk with server. Waiting some seconds ..." << std::endl;
			sleep( SRV_RETRY );
		}
		sleep( 1 );
	}
}

