#ifndef SRV_HH
#define SRV_HH

#include <vector>
#include <string>

int send_cmd( const std::string& s, std::string& r );
int send_cmd( int cmd, int n, ... );
int send_cmd( int cmd, std::vector< std::string >& resp, int n, ... );

#endif

