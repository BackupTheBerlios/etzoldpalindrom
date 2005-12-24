#include <string.h>
#include "diffie_hellman.hh"
#include "settings.hh"

diffie_hellman::diffie_hellman( gmp_randstate_t rstate ) {
	mpz_init_set_str( n, PUBLIC_MODULUS, 10 );
	mpz_init_set_str( g, PUBLIC_GENERATOR, 10 );
	mpz_init( r );
	mpz_init( k );
	mpz_init( x );

	mpz_urandomm( r, rstate, n );
	mpz_powm( x, g, r, n );  // x = (g**r) mod n
}

diffie_hellman::~diffie_hellman() {
	mpz_clear( n );
	mpz_clear( g );
	mpz_clear( k );
	mpz_clear( r );
	mpz_clear( x );
}

void diffie_hellman::get_key1( mpz_t k ) {
	mpz_set( k, x );
}

void diffie_hellman::set_key2( const char* v ) {
	mpz_set_str( k, v, 10 );
}

void diffie_hellman::session_key( mpz_t sk ) {
	mpz_powm( sk, k, r, n );
}

void diffie_hellman::get_des3_key( DES_cblock k1, DES_cblock k2, DES_cblock k3 ) {
	DES_cblock k[ 3 ];
	int p = 0;
	mpz_t sk;
	mpz_init( sk );
	mpz_powm( sk, this->k, r, n );
	for( int i = 0; i < 3; ++i ) {
		memset( k[ i ], 0, sizeof( DES_cblock ) );
		for( int b = 0; b < sizeof( DES_cblock ); ++b ) {
			int n = 0;
			for( int j = 0; j < 7; ++j ) {
				if( mpz_tstbit( sk, p++ ) ) {
					k[ i ][ b ] |= 1;
					++n;
				}
				k[ i ][ b ] <<= 1;
			}
			if( n % 2 == 0 ) { // DES key must have odd parity, make it odd
				k[ i ][ b ] |= 1;
			}
		}
	}
	memcpy( k1, k[ 0 ], sizeof( DES_cblock ) );
	memcpy( k2, k[ 1 ], sizeof( DES_cblock ) );
	memcpy( k3, k[ 2 ], sizeof( DES_cblock ) );
	mpz_clear( sk );
}
