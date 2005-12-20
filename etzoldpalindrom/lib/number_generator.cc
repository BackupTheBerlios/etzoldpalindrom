#include "number_generator.hh"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

number_generator::number_generator( int len ) {
	mpz_init( tmp );
	ones = 0;
	buf = new int[ len ];
	cap = len;
}

number_generator::~number_generator() {
	mpz_clear( tmp );
	delete[] buf;
}

void number_generator::first_number( mpz_t r ) {
	memset( buf, 0, cap * sizeof( int ) );
	buf[ 0 ] = buf[ cap - 1 ] = 1;
	ones = 0;
	mpz_ui_pow_ui( r, 10, cap - 1 );
	mpz_add_ui( r, r, 1 );
}

void number_generator::change( mpz_t p, int pos, bool add ) {
	int mid = cap >> 1;
	if( add ) {
		buf[ pos ]++;
		mpz_ui_pow_ui( tmp, 10, pos );
		mpz_add( p, p, tmp );
	} else {
		buf[ pos ]--;
		mpz_ui_pow_ui( tmp, 10, pos );
		mpz_sub( p, p, tmp );
	}
	if( pos < mid ) {
		change( p, ( mid << 1 ) - pos, add );
	}
}

bool number_generator::next_number( mpz_t p, int n ) {
	int mid = cap >> 1;
	for( ; n > 0; --n ) {
		int i;
		if( buf[ mid ] == 0 ) {
			change( p, mid, true );
		} else {
			if( ones == mid - 1 ) {
				return true;
			}
			change( p, mid, false );
			int nones = ones;
			for( i = mid - 1; i > 0 && buf[ i ] == 1 && nones; --i, --nones );
			if( nones == 0 ) {
				nones = ++ones;
				first_number( p );
				for( i = 0, ones = nones; i < nones; change( p, ++i, true ) );
			} else {
				for( ; buf[ i ] == 0; --i );
				change( p, i, false );
				change( p, i + 1, true );
			}
		}
	}
	return false;
}


