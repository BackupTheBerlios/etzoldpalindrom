#ifndef PAL_LIBPAL_HH
#define PAL_LIBPAL_HH

#ifndef DEFAULT_PRIME_ITERATIONS
#define DEFAULT_PRIME_ITERATIONS 1000
#endif

bool is_prime( const char* p, int iterations = DEFAULT_PRIME_ITERATIONS );
bool is_palindrom( const char* val );

#endif

