#include <iostream>
#include <sstream>
#include "libpal.hh"
#include <gmp.h>

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

int compact( const std::string& p, const char* up = "^", const char* down = " " ) {
	if( p.empty() ) {
		return 1;
	}
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
	std::cout << r.str() << std::endl;
	return 0;
}

int compact_html( const std::string& p ) {
	return compact( p, "<sup>", "</sup>" );
}

void arg_check( int argc, char** argv, int i ) {
	if( i + 1 >= argc || ( argv[ i + 1 ][ 0 ] == '-' ) ) {
		std::cerr << "option " << argv[ i ] << " requires a value" << std::endl;
		exit( 1 );
	}
}

bool arg_exists( int argc, char** argv, const char* opt ) {
	for( int i = 1; i < argc; ++i ) {
		if( strcmp( argv[ i ], opt ) == 0 ) {
			return true;
		}
	}
	return false;
}

char* arg_value( int argc, char** argv, const char* opt ) {
	for( int i = 1; i < argc; ++i ) {
		if( strcmp( argv[ i ], opt ) == 0 ) {
			arg_check( argc, argv, i );
			return argv[ ++i ];
		}
	}
	return NULL;
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
		return compact( val.empty() ? arg_value( argc, argv, "--compact" ) : val );
	}

	if( arg_exists( argc, argv, "--compacthtml" ) ) {
		return compact_html( val.empty() ? arg_value( argc, argv, "--compact_html" ) : val );
	}
}

