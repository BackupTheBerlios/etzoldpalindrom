#include <iostream>
#include <vector>
#include <sstream>

#include "libpal.hh"
#include "settings.hh"
#include "socket.hh"
#include "number_generator.hh"

typedef struct {
	const char* email;
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

config_t c;

void read_config( config_t* c );
void loop();
bool parse_line( const std::string& s, data_t* d );
std::string check( const data_t& data );
void send_palindrom( int fd, const std::string& n, const std::string& id, int retries = 10 );

int main( int argc, char** argv ) {
	read_config( &c );
	loop();
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

void read_config( config_t* c ) {
	memset( c, 0, sizeof( config_t ) );
	FILE* f = fopen( CONFIG_FILE, "r" );
	if( f ) {
		// TODO
		fclose( f );
	} else {
		c->email = "no@email.set";
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
		if( is_palindrom( tmp ) ) {
			if( mpz_probab_prime_p( p, 10 ) > 0 ) {
				printf( "%s = ", tmp );
				mpz_get_str( tmp, 10, p );
				printf( "%s\n", tmp );
				r = tmp;
				break;
			}
		}
		if( gen.next_number( p ) ) {
			break;
		}
	}
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

void loop() {
	std::string id;
	std::string r;
	while( true ) {
		int fd;
		if( ( fd = connect_socket( SRV_ADDR, SRV_PORT ) ) != -1 ) {
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

