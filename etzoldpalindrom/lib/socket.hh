#ifndef SOCKET_HH
#define SOCKET_HH

#include <string>

int init_socket( int port, int queue = 100 );
int accept_socket( int sock );
std::string get_ip( int fd );
bool read_line( int fd, std::string& s, unsigned int maxlen = 1024, int timeout = 30 );
bool send_line( int fd, const std::string& s );
int connect_socket( const char* srv, int port, bool resolvehost = true );

#endif

