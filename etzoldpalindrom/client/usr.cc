#include "usr.hh"
#include "libpal.hh"
#include "settings.hh"
#include "sha2.hh"
#include "srv.hh"
#include <vector>
#include <iostream>

void encode_pwd( std::string& pwd ) {
	unsigned char dig[ SHA512_DIGEST_SIZE ];
	sha512( (unsigned char*) pwd.data(), pwd.size(), dig );
	pwd = hexdump( dig, SHA512_DIGEST_SIZE );
}

void register_account( config_t& c ) {
	std::string pwd2;
	do {
		do {
			std::cout << "Select a nick name (at least 3 characters): " << std::flush;
			std::getline( std::cin, c.nick );
			if( c.nick.find( "[" ) != std::string::npos || c.nick.find( "]" ) != std::string::npos ) {
				std::cout << "  Nick name contains invalid characters." << std::endl;
				c.nick = "";
			}
		} while( c.nick.size() < 3 );
		std::cout << "  Checking whether the nick is already in use ..." << std::endl;
		int r = send_cmd( CMD_NICK, 1, c.nick.c_str() );
		if( r == RET_OK ) {
			std::cout << "  ok" << std::endl;
			break;
		} else if( r == RET_NICKINUSE ) {
			std::cout << "  Sorry, the nick is already in use. Please select another one." << std::endl;
		} else if( r == RET_INVALIDNICK ) {
			std::cout << "The nick is invalid." << std::endl;
		} else {
			std::cout << "  An error occurred. Please try again later." << std::endl;
		}
	} while( true );
	std::cout << "What's your real name: " << std::flush;
	std::getline( std::cin, c.realname );
	do {
		std::cout << "Do you want your real name to be public? [Y/n] " << std::flush;
		std::getline( std::cin, c.pub );
	} while( c.pub != "y" && c.pub != "Y" && c.pub != "n" && c.pub != "N" && ! c.pub.empty() );
	if( c.pub.empty() ) c.pub = "y";
	do {
		do {
			std::cout << "Select a password (at least 6 characters): " << std::flush;
			std::getline( std::cin, c.pwd );
		} while( c.pwd.size() < 6 );
		std::cout << "Verify the password: " << std::flush;
		std::getline( std::cin, pwd2 );
		if( c.pwd == pwd2 ) break;
	} while( true );
	if( send_cmd( CMD_REGISTER, 4, c.nick.c_str(), c.realname.c_str(), c.pub.c_str(), 
			c.pwd.c_str() ) != RET_OK ) {
		std::cout << "Registration failed." << std::endl;
		exit( 1 );
	}
	encode_pwd( c.pwd );
}

void use_account( config_t& c ) {
	std::vector< std::string > resp;
	do {
		std::cout << "What's your nick name (at least 3 characters): " << std::flush;
		std::getline( std::cin, c.nick );
	} while( c.nick.size() < 3 );
	std::cout << "Password: " << std::flush;
	std::getline( std::cin, c.pwd );
	std::cout << "  Contacting server ... " << std::endl;
	int r = send_cmd( CMD_GET_CONFIG, resp, 2, c.nick.c_str(), c.pwd.c_str() );
	if( r == RET_INVALIDLOGIN ) {
		std::cout << "  Invalid nick or password." << std::endl;
		exit( 1 );
	} else if( r != RET_OK ) {
		std::cout << "  Error." << std::endl;
		exit( 1 );
	}
	std::cout << "  ok" << std::endl;
	if( resp.size() != 2 ) {
		std::cout << "  Invalid server response." << std::endl;
		exit( 1 );
	}
	c.realname = resp[ 0 ];
	c.pub = resp[ 1 ];
	encode_pwd( c.pwd );
}


