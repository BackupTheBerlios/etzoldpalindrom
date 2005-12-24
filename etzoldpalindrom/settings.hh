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

/* 1024 bit public modulus (prime) */
#define PUBLIC_MODULUS "148367284749101087230910714576119281512389721358579345512587782137717214642497547128219465527191732647455829183596632946342984973721937371837612987328746193767683756102397528374018230927486587029138923872984652093847191938946561912912984656561919191284652372191983275465591298564219191998184565612919856411637"

#define PUBLIC_GENERATOR "813098567123476128971459012461276675762387129120913278932874686584291012893765932023875202302348576565754828320230233742023010384734874652001191746429328329238795681292398234324274745627672982391298197428729239824379248743873465785872982389239823985654129129834654613984763467887"

#endif
