#include "srv.hh"
#include "settings.hh"
#include "libpal.hh"
#include "socket.hh"
#include <stdarg.h>
#include <sstream>

extern std::string _srv;
extern int _port;
extern bool _ip;

int send_cmd( const std::string& s, std::string& r ) {
	std::string buf;
	int ret = RET_COMERR;
	socket sock;
	if( sock.connect_socket( _srv, _port, _ip ? false : true ) && sock.ssl_init() && sock.ssl() ) {
		sock.send_line( s );
		if( sock.read_line( buf ) && ! buf.empty() ) {
			std::string::size_type p = buf.find( " " );
			std::string rx = buf.substr( 0, p );
			if( ! rx.empty() && rx.find_first_not_of( "0123456789" ) == std::string::npos ) {
				ret = atoi( rx.c_str() );
				buf.erase( 0, p );
				if( p != std::string::npos ) {
					buf.erase( 0, 1 );
				}
				r = buf;
			}
		}
	}
	return ret;
}

int send_cmd( int cmd, int n, ... ) {
	std::stringstream s;
	std::string buf;
	va_list ap;
	va_start( ap, n );
	s << CLIENT_VERSION << " " << cmd;
	for( int i = 0; i < n; ++i ) {
		s << " [" << va_arg( ap, char* ) << "]";
	}
	va_end( ap );
	return send_cmd( s.str(), buf );
}

int send_cmd( int cmd, std::vector< std::string >& resp, int n, ... ) {
	std::stringstream s;
	std::string buf;
	int r;
	va_list ap;
	va_start( ap, n );
	s << CLIENT_VERSION << " " << cmd;
	for( int i = 0; i < n; ++i ) {
		s << " [" << va_arg( ap, char* ) << "]";
	}
	va_end( ap );
	r = send_cmd( s.str(), buf );
	split( buf, resp );
	return r;
}


