#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unistd.h>

#include "libpal.hh"
#include "settings.hh"
#include "socket.hh"
#include "number_generator.hh"
#include "args.hh"

typedef struct {
	std::string email;
} config_t;

typedef struct {
	std::string ip;
	time_t tim;
	std::string version;
	int n;
	int len;
	std::string number;
	std::string id;
	u_int32_t chksum;
} data_t;

std::string _version = CLIENT_VERSION;
std::string _srv = SRV_ADDR;
bool        _ip = false;
int         _port = SRV_PORT;

void read_config( config_t& c );
void write_config( const config_t& c );
void loop( const config_t& c );
bool parse_line( const std::string& s, data_t* d );
std::string check( const data_t& data );
void send_palindrom( int fd, const std::string& n, const std::string& id, int retries = 10 );

int main( int argc, char** argv ) {
	config_t conf;

	if( arg_exists( argc, argv, "--hostip" ) ) {
		_srv = arg_value( argc, argv, "--hostip" );
		_ip = true;
	}
	if( arg_exists( argc, argv, "--port" ) ) {
		_port = atoi( arg_value( argc, argv, "--port" ) );
	}

	std::cout << "etzoldpal " << _version << std::endl;
	read_config( conf );
	std::cout << "using email " << conf.email << std::endl;

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

bool parse_line( const std::string& s, data_t* d ) {
	bool ret = false;
	std::vector< std::string > v;
	for( std::string::size_type i = 0; i < s.size(); ) {
		if( s[ i ] == '[' ) {
			v.push_back( s.substr( i, s.find( "]", i ) - i + 1 ) );
			if( v.back().size() > 1 ) {
				v.back().erase( 0, 1 );
				v.back().erase( v.back().size() - 1 );
			}
			i = s.find( "]", i );
		} else if( ! isspace( s[ i ] ) && s[ i ] != ']' ) {
			v.push_back( s.substr( i, s.find( " ", i ) - i ) );
			i = s.find( " ", i );
		} else {
			++i;
		}
	}
	if( ret = ( v.size() == 8 ) ) {
		d->ip = v[ 0 ];
		d->tim = atoi( v[ 1 ].c_str() );
		d->version = v[ 2 ];
		d->n = atoi( v[ 3 ].c_str() );
		d->len = atoi( v[ 4 ].c_str() );
		d->number = v[ 5 ];
		d->id = v[ 6 ];
		d->chksum = atoll( v[ 7 ].c_str() );

		std::string x( s );
		x.erase( x.find_last_of( " " ) );
		ret = ( d->chksum == chksum( x ) );
	}
	return ret;
}

void write_config( config_t& c ) {
	std::ofstream f( CONFIG_FILE, std::ios::trunc );
	if( ! f.is_open() || f.fail() ) {
		std::cerr << "Could not open configuration file " << CONFIG_FILE << " for writing." << std::endl;
		exit( 1 );
	}
	f << "email: " << c.email << std::endl;
}

void read_config( config_t& c ) {
	std::ifstream f( CONFIG_FILE );
	std::string s;
	if( ! f.is_open() || f.fail() ) {
		printf( 
			"No configuration file was found.\n"
			"Email address: " );
		fflush( stdout );
		std::getline( std::cin, c.email );
		write_config( c );
	} else {
		while( std::getline( f, s ) ) {
			std::string::size_type p = s.find( ": " );
			if( p != std::string::npos ) {
				std::string key = s.substr( 0, p );
				std::string val = s.substr( p );
				val.erase( 0, 2 );
				if( key == "email" ) {
					c.email = val;
				}
			}
		}
	}
}

std::string check( const data_t& data ) {
	std::string r;
	std::string num;
	char* tmp;
	mpz_t c, p;
	
	decompact( data.number, num );
	mpz_init_set_str( p, num.c_str(), 10 );
	mpz_init( c );

	number_generator gen( num );
	mpz_init_set_str( p, num.c_str(), 10 );

	tmp = new char[ 2 * num.size() + 128 ];
	for( int n = 0; n < data.n; ++n ) {
		mpz_mul( c, p, p );
		mpz_get_str( tmp, 10, c );
		if( is_palindrom( tmp ) && mpz_probab_prime_p( p, 10 ) > 0 ) {
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

void send_palindrom( int fd, const std::string& n, const std::string& id, int retries ) {
	std::stringstream s;
	std::string x;
	compact( n, x );
	s << "F " << CLIENT_VERSION << " [" << x << "] " << id;
	while( ! send_line( fd, s.str() ) && retries-- ) {
		sleep( 10 );
	}
}

void loop( const config_t& c ) {
	std::string id;
	std::string r;
	while( true ) {
		int fd;
		if( ( fd = connect_socket( _srv.c_str(), _port, _ip ? false : true ) ) != -1 ) {
			if( ! r.empty() ) {
				send_palindrom( fd, r, id );
				r = "";
				close( fd );
			} else {
				std::stringstream s;
				std::string buf;
				s << "G " << CLIENT_VERSION << " [" << c.email << "]";
				if( send_line( fd, s.str() ) && read_line( fd, buf ) ) {
					data_t data;
					close( fd );
					if( parse_line( buf, &data ) ) {
						r = check( data );
						id = data.id;
					} else {
						std::cerr << "Got corrupt data. Waiting some seconds ..." << std::endl;
						sleep( SRV_RETRY );
					}
				} else {
					close( fd );
					std::cerr << "Could not talk with server. Waiting some seconds ..." << std::endl;
					sleep( SRV_RETRY );
				}
			}
		} else {
			std::cerr << "Connecting to server failed. Waiting some seconds ..." << std::endl;
			sleep( SRV_RETRY );
		}
	}
}

