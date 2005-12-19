#ifndef NUMBER_GENERATOR_HH
#define NUMBER_GENERATOR_HH

#include <gmp.h>

class number_generator {
public:
	number_generator( int len );
	virtual ~number_generator();

	void first_number( mpz_t r );
	bool next_number( mpz_t p );

protected:
	void change( mpz_t p, int pos, bool add );

	int ones;
	int cap;
	int* buf;
	mpz_t tmp;
};

#endif

