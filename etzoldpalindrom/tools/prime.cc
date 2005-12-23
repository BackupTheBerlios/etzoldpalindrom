#include <iostream>
#include <sstream>
#include <gmp.h>
#include "args.hh"

int main( int argc, char** argv ) {
	mpz_t p;
	mpz_t q;

	mpz_init_set_str( p, argv[ 1 ], 10 );
	mpz_init( q );
	mpz_nextprime( q, p );

	gmp_printf( "%Zd\n", q );
	printf( "bits: %d\n", mpz_sizeinbase( q, 2 ) );
}

