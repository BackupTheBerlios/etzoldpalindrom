#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include "logging.hh"

const int logging::_fatal = 0;
const int logging::_error = 1;
const int logging::_warning = 2;
const int logging::_notice = 3;
const int logging::_debug = 4;
const char* logging::_log[ 5 ] = { "fatal\0", "error\0", "warning\0", "notice\0", "debug\0" };

logging::logging( const std::string& name, int level ) : _level( level ), _name( name ) {
	_tostdout = false;
}

logging::logging( const std::string& name, const std::string& path, int level ) 
	: _level( level ), _name( name ), _path( path ) {
	_tostdout = false;
}

void logging::set_level( int level ) {
	_level = level;
}

void logging::fatal( const char* fmt, ... ) {
	va_list ap;
	va_start( ap, fmt );
	log( _fatal, fmt, ap );
	va_end( ap );
}

void logging::error( const char* fmt, ... ) {
	va_list ap;
	va_start( ap, fmt );
	log( _error, fmt, ap );
	va_end( ap );
}

void logging::warning( const char* fmt, ... ) {
	va_list ap;
	va_start( ap, fmt );
	log( _warning, fmt, ap );
	va_end( ap );
}

void logging::notice( const char* fmt, ... ) {
	va_list ap;
	va_start( ap, fmt );
	log( _notice, fmt, ap );
	va_end( ap );
}

void logging::debug( const char* fmt, ... ) {
	va_list ap;
	va_start( ap, fmt );
	log( _debug, fmt, ap );
	va_end( ap );
}

void logging::log( int level, const char* fmt, va_list ap ) {
	if( level <= _level ) {
		char buff[ 1024 ];
		time_t ct = time( NULL );
		struct tm* t = localtime( &ct );
		strftime( buff, 1023, "%Y%m%d", t );
		char filename[ 10240 ];

		if( ! _path.empty() ) {
			sprintf( filename, "%s/%s-%s.log", _path.c_str(), buff, _name.c_str() );
		} else {
			sprintf( filename, "%s-%s.log", buff, _name.c_str() );
		}
		FILE* f = fopen( filename, "at" );
		if( f ) {
			chmod( filename, 0666 );
			strftime( buff, 1013, "%H:%M:%S", t );
			sprintf( filename, "%s [%i] %s/", buff, getpid(), _log[ level ] );
			vsprintf( filename + strlen( filename ), fmt, ap );
			fputs( filename, f );
			fputs( "\n", f );
			if( _tostdout ) {
				std::cout << filename << std::endl;
			}
			fclose( f );
		}
	}
}

void logging::set_stdout( bool b ) {
	_tostdout = b;
}

void logging::set_filename( const std::string& fname ) {
	_name = fname;
}

void logging::set_path( const std::string& path ) {
	_path = path;
}
