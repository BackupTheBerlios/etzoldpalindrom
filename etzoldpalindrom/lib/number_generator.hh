#ifndef NUMBER_GENERATOR_HH
#define NUMBER_GENERATOR_HH

#include <string>
#include <gmp.h>

class number_generator {
public:
	number_generator( int len );
	number_generator( const std::string& num );
	virtual ~number_generator();

	void first_number( mpz_t r );
	bool next_number( mpz_t p, int n = 1 );

protected:
	void change( mpz_t p, int pos, bool add );

	int ones;
	int cap;
	int* buf;
	mpz_t tmp;
};

#endif

