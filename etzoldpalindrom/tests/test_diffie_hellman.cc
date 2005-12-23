#include "diffie_hellman.hh"
#include <assert.h>

int main( int argc, char** argv ) {
	char buf[ 4096 ];
	mpz_t alpha, beta, ska, skb;
	gmp_randstate_t rstate;

	gmp_randinit( rstate, GMP_RAND_ALG_LC, 32L );
	diffie_hellman a( rstate );
	diffie_hellman b( rstate );

	mpz_init( alpha );
	mpz_init( beta );
	a.get_key1( alpha );
	b.get_key1( beta );
	mpz_get_str( buf, 10, beta );
	a.set_key2( buf );
	mpz_get_str( buf, 10, alpha );
	b.set_key2( buf );

	mpz_init( ska );
	mpz_init( skb );

	a.session_key( ska );
	b.session_key( skb );
	assert( mpz_cmp( ska, skb ) == 0 );

	gmp_printf( "alpha = %Zd\n\n", alpha );
	gmp_printf( "beta = %Zd\n\n", beta );
	gmp_printf( "session key = %Zd\n\n", ska );

	gmp_randclear( rstate );
}
