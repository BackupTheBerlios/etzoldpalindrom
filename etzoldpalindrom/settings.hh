#ifndef SETTINGS_HH
#define SETTINGS_HH

#ifndef SRV_PORT
#define SRV_PORT 23745
#endif

#ifndef SRV_ADDR
#define SRV_ADDR "dzdev"
#endif

#ifndef SRV_RETRY
#define SRV_RETRY 30
#endif

#ifndef CLIENT_VERSION
#define CLIENT_VERSION "1.0"
#endif

#ifndef CONFIG_FILE
#define CONFIG_FILE "config"
#endif

#ifndef START_LEN
#define START_LEN 1023
#endif

#ifndef NUMBERS
#define NUMBERS 10
#endif

#ifndef MAX_REQUESTS_PER_MINUTE
#define MAX_REQUESTS_PER_MINUTE 10
#endif

#ifndef SRV_LOG_FILE
#define SRV_LOG_FILE "epalsrv"
#endif

#ifndef SRV_DATA_PATH
#define SRV_DATA_PATH "."
#endif

#ifndef DB_AUTH
#define DB_AUTH "auth.db"
#endif

#define RET_OK            0
#define RET_ERR           1
#define RET_COMERR        2
#define RET_NICKINUSE     3
#define RET_INVALIDNICK   4
#define RET_INVALIDLOGIN  5

#define CMD_NICK          1
#define CMD_REGISTER      2
#define CMD_GET_CONFIG    3

#endif
