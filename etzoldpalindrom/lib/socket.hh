#ifndef SOCKET_HH
#define SOCKET_HH

#include <string>
#include <openssl/ssl.h>

class socket {
public:
	socket( int fd );
	socket();
	virtual ~socket();
	
	void close();
	void set_fd( int fd );
	std::string get_ip() const;
	bool read_line( std::string& s, unsigned int maxlen = 1024, int timeout = 30 ) const;
	bool send_line( const std::string& s ) const;
	bool connect_socket( const std::string& srv, int port, bool resolvehost = true );
	bool ssl();
	bool ssl_init( bool client = true );

	static std::string get_ip( int fd );
protected:
	int _fd;
	SSL_CTX* _ctx;
	SSL* _ssl;
	bool _sslclient;
};

int init_socket( int port, int queue = 100 );
int accept_socket( int sock );

#endif

