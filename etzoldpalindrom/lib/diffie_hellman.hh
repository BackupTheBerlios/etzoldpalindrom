#ifndef DIFFIE_HELLMAN_HH
#define DIFFIE_HELLMAN_HH

#include <gmp.h>
#include <openssl/des.h>

class diffie_hellman {
public:
	diffie_hellman( gmp_randstate_t );
	virtual ~diffie_hellman();
	
	/*! Computes g**r mod n 
		\param k Receives the key and must be initialzed by the caller.
	*/
	void get_key1( mpz_t k );

	void set_key2( const char* k );

	/*! Computes the session key.
		\param k Receives the session key and must be initialized by the caller.
	*/
	void session_key( mpz_t k );

	void diffie_hellman::get_des3_key( DES_cblock k1, DES_cblock k2, DES_cblock k3 );

protected:
	mpz_t n;
	mpz_t g;
	mpz_t r;
	mpz_t k;  // alpha or beta
	mpz_t x;  // (g**r) mod n
};

#endif


