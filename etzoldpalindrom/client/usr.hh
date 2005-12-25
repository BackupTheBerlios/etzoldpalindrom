#ifndef USR_HH
#define USR_HH

#include "types.hh"
#include <string>

void encode_pwd( std::string& pwd );
void register_account( config_t& c );
void use_account( config_t& c );

#endif

