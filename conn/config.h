#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DBG_MODE 1

#ifdef DBG_MODE
    #define DEBUG(x) \
        do{ perror((x)); }while(0)
#endif

#define PROT_HTTP  0
#define PROT_HTTPS 1

#define HTTP_BIND_PORT 9090
#define HTTPS_BIND_PORT 9091

typedef int file_type;

#define FILE_NONE 0
#define FILE_JPEG 1
#define FILE_GIF  2
#define FILE_PNG  3

#endif /* _CONFIG_H_ */
