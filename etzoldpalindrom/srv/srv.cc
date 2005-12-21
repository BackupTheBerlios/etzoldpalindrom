#include <iostream>
#include <string>
#include <sstream>
#include <assert.h>

#include "socket.hh"
#include "number_generator.hh"
#include "args.hh"
#include "libpal.hh"
#include "settings.hh"

std::string get_id( int len = 10 );

int main( int argc, char** argv ) {
	int len = START_LEN;
	mpz_t x;

	if( arg_exists( argc, argv, "--start" ) ) {
		len = atoi( arg_value( argc, argv, "--start" ) );
	}

	srand( time( NULL ) );
	mpz_init( x );
	int sock = init_socket( SRV_PORT );
	std::cout << "listening on port " << SRV_PORT << " ..." << std::endl;
	for( ; ; len += 2 ) {
		std::cout << "len=" << len << std::endl;
		number_generator gen( len );
		gen.first_number( x );

		char* tmp = new char[ len + 128 ];
		do {
			int fd;
			std::string s, ip;

			assert( ( fd = accept_socket( sock ) ) != -1 );
			read_line( fd, s );

			ip = get_ip( fd );
			std::cout << "rcv: " << ip << " " << time( NULL ) << " " << s << std::endl;
			if( ! s.empty() ) {
				if( s[ 0 ] == 'F' ) { // client found an etzold palindrom
					close( fd );
					break;
				}
				mpz_get_str( tmp, 10, x );
				compact( tmp, s );
				std::stringstream out;
				out << ip << " " << time( NULL ) << " 1.0 " << NUMBERS << " " << len << " [" << s << "] " << get_id();
				u_int32_t chk = chksum( out.str() );
				out << " " << chk;
				std::cout << "snd: " << out.str() << std::endl;
				send_line( fd, out.str() );
			}
			std::cout << "log: " << ip << " " << time( NULL ) << " closing connection" << std::endl;
			close( fd );
		} while( ! gen.next_number( x, NUMBERS ) );
		delete[] tmp;
	}
}

std::string get_id( int len ) {
	static const char* sigma = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string s;
	for( int l = strlen( sigma ); len; --len ) {
		s += sigma[ rand() % l ];
	}
	return s;
}

