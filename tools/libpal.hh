#ifndef PAL_LIBPAL_HH
#define PAL_LIBPAL_HH

#ifndef DEFAULT_PRIME_ITERATIONS
#define DEFAULT_PRIME_ITERATIONS 10
#endif

#include <gmp.h>

bool is_prime( const char* p, int iterations = DEFAULT_PRIME_ITERATIONS );
bool millerrabin( const char* val, int it = DEFAULT_PRIME_ITERATIONS );
bool millerrabin( mpz_t n, int it = DEFAULT_PRIME_ITERATIONS );

bool is_palindrom( const char* val );

#endif

