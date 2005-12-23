#ifndef LOGGING_HH
#define LOGGING_HH

#include <string>
#include <stdarg.h>

class logging {
public:
	logging( const std::string& name, int level = _notice );
	logging( const std::string& name, const std::string& path, int level = _notice );

	void set_filename( const std::string& name );
	void set_path( const std::string& path );
	void set_stdout( bool );
	void set_level( int level );

	void warning( const char* fmt, ... );
	void notice( const char* fmt, ... );
	void fatal( const char* fmt, ... );
	void debug( const char* fmt, ... );
	void error( const char* fmt, ... );

	static const int _fatal;
	static const int _error;
	static const int _warning;
	static const int _notice;
	static const int _debug;

protected:
	void log( int level, const char* format, va_list ap );

	std::string _name;
	std::string _path;
	int _level;
	static const char* _log[ 5 ];
	bool _tostdout;
};

#endif
