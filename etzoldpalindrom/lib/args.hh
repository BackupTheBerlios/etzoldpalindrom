#ifndef ARGS_HH
#define ARGS_HH

void arg_check( int argc, char** argv, int i );
bool arg_exists( int argc, char** argv, const char* opt );
char* arg_value( int argc, char** argv, const char* opt );

#endif

