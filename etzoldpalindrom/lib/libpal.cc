#include "libpal.hh"
#include <string.h>
#include <sstream>

bool millerrabin( const char* val, int it ) {
	mpz_t n;
	bool r;
	mpz_init_set_str( n, val, 10 );
	r = millerrabin( n, it );
	mpz_clear( n );
	return r;
}

bool millerrabin( mpz_t n, int it ) {
	mpz_t nm1, q, x, y;
	mpz_init( nm1 );
	mpz_init( q );
	mpz_init( x );
	mpz_init( y );
	mpz_sub_ui( nm1, n, 1L );                    // nm1 = n - 1
	// n - 1 = nm1 = 2**k * q
	unsigned long int k = mpz_scan1( nm1, 0L );  // search first bit being set
	mpz_tdiv_q_2exp( q, nm1, k );                // right shift
	gmp_randstate_t rstate;
	gmp_randinit( rstate, GMP_RAND_ALG_DEFAULT, 32L );
	bool isprime = true;
	for( int i = 0; i < it && isprime; ) {
		do {
			mpz_urandomb( x, rstate, mpz_sizeinbase( n, 2 ) - 1 );
		} while( mpz_cmp_ui( x, 1L ) <= 0 );
		mpz_powm( y, x, q, n );                 // y = (x**q) mod n
		if( ! ( mpz_cmp_ui( y, 1L ) == 0 || mpz_cmp( y, nm1 ) == 0 ) ) {
			for( int j = 1; j < k; ++j ) {
				mpz_powm_ui( y, y, 2L, n );     // y = y**2 mod n
				if( mpz_cmp( y, nm1 ) == 0 ) {
					goto primeexit;
				} else if( mpz_cmp_ui( y, 1L ) == 0 ) {
					break;
				}
			}
			isprime = false;
		}
primeexit:
		++i;
	}
	gmp_randclear( rstate );
	mpz_clear( y );
	mpz_clear( x );
	mpz_clear( q );
	mpz_clear( nm1 );
	return isprime;
}

bool is_prime( const char* p, int it ) {
	return millerrabin( p, it );
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

void compact( const std::string& p, std::string& out, const char* up, const char* down ) {
	if( ! p.empty() ) {
		int c = 1;
		char l = p[ 0 ];
		std::stringstream r;
		#define ADD \
			r << l; \
			if( c == 2 ) { \
				r << l; \
			} else { \
				if( c > 1 ) { \
					r << up << c << down; \
				} \
			}
		for( unsigned int i = 1; i < p.size(); ++i, ++c ) {
			if( p[ i ] != l ) {
				ADD
				l = p[ i ], c = 0;
			}
		}
		ADD
		out = r.str();
	}
}

void decompact( const std::string& p, std::string& d ) {
	if( ! p.empty() ) {
		std::stringstream s;
		for( std::string::size_type i = 0; i < p.size(); ++i ) {
			if( p[ i ] == '^' ) {
				char l = ( i > 0 ? p[ i - 1 ] : '.' );
				std::string::size_type x = ++i;
				for( ; i < p.size() && ! isspace( p[ i ] ); ++i );
				if( x < p.size() ) {
					for( int n = atoi( p.substr( x, i - x ).c_str() ) - 1; n; --n ) {
						s << l;
					}
				}
			} else {
				s << p[ i ];
			}
		}
		d = s.str();
	}
}

u_int32_t chksum( const std::string& s ) {
	u_int32_t r = 0;
	for( unsigned int i = 0; i < s.size(); ++i ) {
		r = r * 5 + s[ i ];
	}
	return r;
}

void split( const std::string& s, std::vector< std::string >& v ) {
	for( std::string::size_type i = 0; i < s.size(); ) {
		if( s[ i ] == '[' ) {
			v.push_back( s.substr( i, s.find( "]", i ) - i + 1 ) );
			if( v.back().size() > 1 ) {
				v.back().erase( 0, 1 );
				v.back().erase( v.back().size() - 1 );
			}
			i = s.find( "]", i );
		} else if( ! isspace( s[ i ] ) && s[ i ] != ']' ) {
			v.push_back( s.substr( i, s.find( " ", i ) - i ) );
			i = s.find( " ", i );
		} else {
			++i;
		}
	}

}

std::string hexdump( const unsigned char* data, int len ) {
	std::string r;
	char buf[ 3 ];
	for( ; len > 0; --len, ++data ) {
		snprintf( buf, 2, "%02x", *data );
		r += buf;
	}
	return r;
}

