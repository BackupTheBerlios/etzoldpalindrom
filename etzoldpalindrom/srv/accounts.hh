#ifndef ACCOUNTS_HH
#define ACCOUNTS_HH

#include <string>

/*!
	\retval RET_INVALIDNICK
	\retval RET_NICKINUSE
	\retval RET_ERR
	\retval RET_OK
*/
int account_check_nick( const std::string& nick );

/*!
	\retval RET_INVALIDNICK
	\retval RET_NICKINUSE
	\retval RET_ERR
	\retval RET_OK
*/
int account_register( const std::string& nick, const std::string& real, const std::string& pub,
		const std::string& pwd );

/*!
	\retval RET_INVALIDLOGIN
	\retval RET_ERR
	\retval RET_OK
*/
int account_get_config( const std::string& nick, const std::string& pwd, std::string& real,
		std::string& pub );

#endif

