#include <iostream>
#include <string>
#include <sstream>

#include "socket.hh"
#include "number_generator.hh"
#include "args.hh"
#include "libpal.hh"

#ifndef PORT
#define PORT 23745
#endif

#ifndef START_LEN
#define START_LEN 1023
#endif

#ifndef NUMBERS
#define NUMBERS 10
#endif

int main( int argc, char** argv ) {
	int len = START_LEN;
	mpz_t x;

	if( arg_exists( argc, argv, "--start" ) ) {
		len = atoi( arg_value( argc, argv, "--start" ) );
	}

	mpz_init( x );
	int sock = init_socket( PORT );
	for( ; ; len += 2 ) {
		std::cout << "len=" << len << std::endl;
		number_generator gen( len );
		gen.first_number( x );

		char* tmp = new char[ len + 128 ];
		do {
			int fd;
			std::string s, ip;
			std::stringstream out;

			assert( ( fd = accept_socket( sock ) ) != -1 );
			read_line( fd, s );

			ip = get_ip( fd );
			std::cout << "rcv: " << ip << " " << time( NULL ) << " " << s << std::endl;
			if( ! s.empty() ) {
				if( s[ 0 ] == 'F' ) { // found an etzold palindrom
					close( fd );
					break;
				}
				mpz_get_str( tmp, 10, x );
				compact( tmp, s );
				out << ip << " " << time( NULL ) << " 1.0 " << NUMBERS << " " << len << " [" << s << "] ";
				out << chksum( out.str() );
				std::cout << "snd: " << out.str() << std::endl;
				send_line( fd, out.str() );
			}
			close( fd );
		} while( ! gen.next_number( x, NUMBERS ) );
		delete[] tmp;
	}
}

