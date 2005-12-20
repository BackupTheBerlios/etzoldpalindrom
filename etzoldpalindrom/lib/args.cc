#include <iostream>
#include "args.hh"

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


