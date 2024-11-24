#ifndef __server_defined
#define __server_defined 1
#endif

#include <fcntl.h>
#include <stdio.h>


#define LINELEN      256
#define TOKENLEN     64  //  len of one arg parametr in struct option
#define ARGV_CHUNK   16  //  number of arguments
#define DOMAINLEN     256

/* #define total_options ({                                                 \
                    char **p;int total = 0;                                 \
                    for (p = ALLOWED_OPTIONS; *p != NULL; p++, total++){;}  \
                    total;                                                  \
                    })
 */

int parse_config(char *path);


char *ALLOWED_OPTIONS[] = {
    "master",
    "blacklist",
    "type",
    NULL
};

struct option {
    char *command;
    char **argv;
    int argc;
    int is_eof;
};

struct bl {
    char **domains;
    int domainc;
};

int total_options(void){
    char **p;
    int total = 0;
    for (p = ALLOWED_OPTIONS; *p != NULL; p++, total++){
        ;
    }
    return total;
};