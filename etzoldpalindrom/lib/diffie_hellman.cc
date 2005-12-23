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

