#include <iostream>
#include <sstream>
#include <gmp.h>
#include "libpal.hh"
#include "args.hh"

#define DIGITS( o ) \
	std::cout << "digits: " << p.size() << std::endl;

class millerrabin_threaded {
public:
	millerrabin_threaded( int threads );
protected:
};

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

int to_compact( const std::string& p, const char* up = "^", const char* down = " " ) {
	std::string x;
	compact( p, x, up, down );
	std::cout << x << std::endl;
	return 0;
}

int to_compact_html( const std::string& p ) {
	return to_compact( p, "<sup>", "</sup>" );
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
		return to_compact( val.empty() ? arg_value( argc, argv, "--compact" ) : val );
	}

	if( arg_exists( argc, argv, "--compacthtml" ) ) {
		return to_compact_html( val.empty() ? arg_value( argc, argv, "--compact_html" ) : val );
	}
}

