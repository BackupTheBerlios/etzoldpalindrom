#ifndef PAL_LIBPAL_HH
#define PAL_LIBPAL_HH

#ifndef DEFAULT_PRIME_ITERATIONS
#define DEFAULT_PRIME_ITERATIONS 10
#endif

#include <sys/types.h>
#include <gmp.h>

bool is_prime( const char* p, int iterations = DEFAULT_PRIME_ITERATIONS );
bool millerrabin( const char* val, int it = DEFAULT_PRIME_ITERATIONS );
bool millerrabin( mpz_t n, int it = DEFAULT_PRIME_ITERATIONS );

bool is_palindrom( const char* val );

void compact( const std::string& p, std::string& out, const char* up = "^", const char* down = " " );
void decompact( const std::string& p, std::string& d );

u_int32_t chksum( const std::string& s );

#endif

