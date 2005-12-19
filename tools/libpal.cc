#include "libpal.hh"
#include <string.h>
#include <gmp.h>

bool is_prime( const char* p, int iterations ) {
	mpz_t n;
	mpz_init_set_str( n, p, 10 );
	return ( mpz_probab_prime_p( n, iterations ) > 0 );
}

bool is_palindrom( const char* val ) {
	if( strlen( val ) > 1 ) {
		size_t n = 0;
		for( size_t p = strlen( val ) - 1; p > n; --p, ++n ) {
			if( val[ p ] != val[ n ] ) {
				return false;
			}
		}
	}
	return true;
}

