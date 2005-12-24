#include "diffie_hellman.hh"
#include <assert.h>
#include <iostream>

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


	DES_cblock k1, k2, k3;
	DES_key_schedule ks1, ks2, ks3;
	DES_cblock ivec;
	char in[ 128 ];
	char out[ 128 ];

	memset( ivec, 0, sizeof( DES_cblock ) );
	a.get_des3_key( k1, k2, k3 );
	DES_set_key_unchecked( &k1, &ks1 );
	DES_set_key_unchecked( &k2, &ks2 );
	DES_set_key_unchecked( &k3, &ks3 );

	strcpy( in, "12345678" );
	DES_ede3_cbc_encrypt( (const unsigned char*) in, (unsigned char*) out, strlen( in ), &ks1, &ks2, &ks3, &ivec, DES_ENCRYPT );
	memset( in, 0, sizeof( in ) );
	memset( ivec, 0, sizeof( DES_cblock ) );
	DES_ede3_cbc_encrypt( (const unsigned char*) out, (unsigned char*) in, 8, &ks1, &ks2, &ks3, &ivec, DES_DECRYPT );
	
	assert( memcmp( in, "12345678", 8 ) == 0 );
}
