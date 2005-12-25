#include "accounts.hh"
#include "settings.hh"
#include <sstream>
#include <vector>
#include <gdbm.h>
#include <iostream>

extern std::string _data_path;

namespace {
	void deserialize( const std::string& s, std::vector< std::string >& v ) {
		for( std::string::size_type i = 0; i < s.size(); ) {
			unsigned char l = s[ i++ ];
			if( i < s.size() ) {
				v.push_back( s.substr( i, l ) );
				i += l;
			}
		}
	}
}

int account_check_nick( const std::string& nick ) {
	datum k;
	int r;
	if( nick.size() < 3 || nick.size() > 64 ) {
		return RET_INVALIDNICK;
	}
	GDBM_FILE dbf = gdbm_open( (char*) ( _data_path + DB_AUTH ).c_str(), 
		4096, GDBM_WRCREAT | GDBM_SYNC, 0666, 0 );
	if( ! dbf ) {
		return RET_ERR;
	}
	k.dptr = (char*) nick.data();
	k.dsize = nick.size();
	r = gdbm_exists( dbf, k );
	gdbm_close( dbf );
	return ( r ? RET_NICKINUSE : RET_OK );
}

int account_register( const std::string& nick, const std::string& real, const std::string& pub,
		const std::string& pwd ) {
	datum k;
	datum d;
	int r;
	if( real.size() > 128 || pub.size() > 1 || pwd.size() > 64 || pwd.size() < 6 ) {
		return RET_ERR;
	}
	if( ( r = account_check_nick( nick ) ) != RET_OK ) {
		return r;
	}
	GDBM_FILE dbf = gdbm_open( (char*) ( _data_path + DB_AUTH ).c_str(),
		4096, GDBM_WRCREAT | GDBM_SYNC, 0666, 0 );
	if( ! dbf ) {
		return RET_ERR;
	}
	std::string publ = ( pub.empty() ? "y" : pub );
	std::stringstream ss;
	std::string s;
	ss << (unsigned char) real.size() << real << (unsigned char) publ.size() << publ 
		<< (unsigned char) pwd.size() << pwd;
	s = ss.str();
	d.dptr = (char*) s.data();
	d.dsize = s.size();
	k.dptr = (char*) nick.data();
	k.dsize = nick.size();
	r = gdbm_store( dbf, k, d, GDBM_INSERT );
	gdbm_close( dbf );
	return ( r ? RET_ERR : RET_OK );
}

int account_get_config( const std::string& nick, const std::string& pwd, std::string& real,
		std::string& pub ) {
	datum k;
	datum d;
	int r = RET_ERR;
	if( nick.size() < 3 || nick.size() > 64 || pwd.size() < 6 || pwd.size() > 64 ) {
		return RET_ERR;
	}
	GDBM_FILE dbf = gdbm_open( (char*) ( _data_path + DB_AUTH ).c_str(),
		4096, GDBM_WRCREAT | GDBM_SYNC, 0666, 0 );
	if( ! dbf ) {
		return RET_ERR;
	}
	k.dptr = (char*) nick.data();
	k.dsize = nick.size();
	d = gdbm_fetch( dbf, k );
	if( d.dptr ) {
		std::vector< std::string > v;
		deserialize( std::string( d.dptr, d.dsize ), v );
		if( v.size() == 3 ) {
			if( v[ 2 ] == pwd ) {
				r = RET_OK;
				real = v[ 0 ];
				pub = v[ 1 ];
			} else {
				r = RET_INVALIDLOGIN;
			}
		}
	} else {
		r = RET_INVALIDLOGIN;
	}
	gdbm_close( dbf );
	return r;
}

