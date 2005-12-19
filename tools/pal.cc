#include <iostream>
#include <string>
#include <assert.h>
#include <pthread.h>
#include <gmp.h>

#include "libpal.hh"
#include "number_generator.hh"

#define MAX_PRIME_DIGITS     ( 10 * 1024 * 1024 )

u_int32_t _cnt = 0;
u_int32_t _pcnt = 0;

void* timer( void* ) {
	u_int32_t start = _cnt;
	time_t startt = time( NULL );
	while( true ) {
		sleep( 30 );
		printf( "numbers tested [%d], palindroms [%d], numbers/s [%.3f]\n", 
			_cnt, _pcnt, 
			(double) ( _cnt - start ) / (double) ( time( NULL ) - startt ) );
		fflush( stdout );
	}
}

int main( int argc, char** argv ) {
	assert( argc > 1 );
	assert( atoi( argv[ 1 ] ) % 2 != 0 );

	pthread_attr_t attr;
	pthread_t thread;

	pthread_attr_init( &attr );
	assert( pthread_create( &thread, &attr, timer, NULL ) == 0 );

	mpz_t p;
	mpz_t c;
	mpz_init( p );
	mpz_init( c );

	for( int len = atoi( argv[ 1 ] ); ; len = ( len << 1 ) + 1 ) {
		number_generator gen( len );
		char* tmp = new char[ 2 * len + 128 ];

		std::cout << "testing length: " << len << std::endl;
		gen.first_number( p );

		do {
			mpz_mul( c, p, p );
			mpz_get_str( tmp, 10, c );
			if( ++_cnt && is_palindrom( tmp ) && ++_pcnt ) {
				std::cout << "." << std::flush;
				if( mpz_probab_prime_p( p, 10 ) > 0 ) {
					printf( "\nXXX %s = ", tmp );
					mpz_get_str( tmp, 10, p );
					printf( "%s\n", tmp );
					break;
				}
				std::cout << "_" << std::flush;
			}
		} while( ! gen.next_number( p ) );

		delete[] tmp;
	}
}
