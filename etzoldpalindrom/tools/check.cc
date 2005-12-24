#include <iostream>
#include <sstream>
#include <gmp.h>
#include "libpal.hh"
#include "args.hh"

#define DIGITS( o ) \
	std::cout << "digits: " << o.size() << std::endl;

int check_prime( const std::string& p, int iterations = DEFAULT_PRIME_ITERATIONS ) {
	int r = 0;
	DIGITS( p )
	std::cout << "checking prime ... " << std::flush;
	if( is_prime( p.c_str(), iterations ) ) {
		std::cout << "ok (with probability 1-2^-" << iterations << ")" << std::endl;
	} else {
		std::cout << "failed" << std::endl;
		r = 1;
	}
	return r;
}

int check_palindrom( const std::string& p ) {
	int r = 0;
	DIGITS( p )
	std::cout << "checking palindrom ... " << std::flush;
	if( is_palindrom( p.c_str() ) ) {
		std::cout << "ok" << std::endl;
	} else {
		std::cout << "failed" << std::endl;
		r = 1;
	}
	return r;
}

int to_compact( const std::string& p, bool html = false ) {
	std::string x;
	compact( p, x, html ? "<sup>" : "^", html ? "</sup>" : " " );
	std::cout << x << std::endl;
	return 0;
}

int to_decompact( const std::string& p ) {
	std::string l;
	decompact( p, l );
	std::cout << l << std::endl;
	return 0;
}

int to_sqr( const std::string& p, bool html = false ) {
	std::string r;
	DIGITS( p );
	to_compact( p, html );
	mpz_t x;
	mpz_t y;
	mpz_init_set_str( x, p.c_str(), 10 );
	mpz_init( y );
	mpz_mul( y, x, x );
	r = mpz_get_str( NULL, 10, y );
	DIGITS( r );
	return to_compact( r, html );
}

int main( int argc, char** argv ) {
	int iterations = DEFAULT_PRIME_ITERATIONS;
	std::string val;
	char* s;

	if( arg_exists( argc, argv, "--stdin" ) ) {
		std::getline( std::cin, val );
	}

	if( arg_exists( argc, argv, "-i" ) ) {
		iterations = atoi( arg_value( argc, argv, "-i" ) );
	}

	if( arg_exists( argc, argv, "--prime" ) ) {
		return check_prime( val.empty() ? arg_value( argc, argv, "--prime" ) : val, iterations );
	}

	if( arg_exists( argc, argv, "-p" ) ) {
		return check_palindrom( val.empty() ? arg_value( argc, argv, "-p" ) : val );
	}

	if( arg_exists( argc, argv, "--compact" ) ) {
		return to_compact( val.empty() ? arg_value( argc, argv, "--compact" ) : val,
			arg_exists( argc, argv, "--html" ) );
	}

	if( arg_exists( argc, argv, "--decompact" ) ) {
		return to_decompact( val.empty() ? arg_value( argc, argv, "--decompact" ) : val );
	}

	if( arg_exists( argc, argv, "--sqr" ) ) {
		return to_sqr( val.empty() ? arg_value( argc, argv, "--sqr" ) : val, 
			arg_exists( argc, argv, "--html" ) );
	}
}

