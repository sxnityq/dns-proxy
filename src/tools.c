#include "include/common_types.h"
#include "include/tools.h"

#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define FQDN_MAXLEN     255


int check_ipv4(char *ipv4){
    
    char *p;
    int listed_octets, walker;
    int o1 = 0, o2 = 0, o3 = 0;

    listed_octets = 1;
    p = ipv4;

    while(listed_octets <= 4){
        for (walker = 1; walker < 4; walker++){
            if (!isdigit(*p)){
                break;
            }
            if (walker == 1){
                o1 = *p - '0';
            } else if (walker == 2){
                o2 = *p - '0';
            } else {
                o3 = *p - '0';
            }
            p++;
        }

        if (walker == 1){
            return 0;
        }

        if (walker == 3){
            o1 *= 10;
        }

        if (walker == 4){
            o1 *= 100;
            o2 *= 10;
        } 

        if (listed_octets < 4){
            if (*p != '.'){
                return 0;
            }
        }

        if ( (o1 + o2 + o3) > 255){
            return 0;
        
        }

        if (listed_octets == 4){
            if (*p == '\0'){
                return 1;
            }
        }

        o1 = 0, o2 = 0, o3 = 0;
        listed_octets++, p++;
    }

    return 0;
}

/* converts ipv4 in string format like "127.0.0.1" to int */
int src_ipv4to_int(char *ipv4){
    
    char *p;
    int listed_octets, octet_sum, walker;
    int o1 = 0, o2 = 0, o3 = 0;

    listed_octets = 1;
    p = ipv4;

    union Ipv4 tmp_ipv4;

    while(listed_octets <= 4){
        for (walker = 1; walker < 4; walker++){
            if (!isdigit(*p)){
                break;
            }
            if (walker == 1){
                o1 = *p - '0';
            } else if (walker == 2){
                o2 = *p - '0';
            } else {
                o3 = *p - '0';
            }
            p++;
        }

        if (walker == 1){
            return 0;
        }

        if (walker == 3){
            o1 *= 10;
        }

        if (walker == 4){
            o1 *= 100;
            o2 *= 10;
        } 
        if (listed_octets < 4){
            if (*p != '.'){
                return -1;
            }
        }

        octet_sum = (o1 + o2 + o3);
        if ( octet_sum > 255){
            return -1;
        
        }

        tmp_ipv4.octets[listed_octets - 1] = octet_sum;

        if (listed_octets == 4){
            if (*p == '\0'){
                return tmp_ipv4.ipv4;
            }
        }

        o1 = 0, o2 = 0, o3 = 0;
        listed_octets++, p++;
    }
    return -1;
}

/* 
 *  0 - IF VALID
 *  FQDN only with(out) trailing dot  
 *  BOTH ARE VALID:
 *  google.com
 *  google.com.
 */

int check_domain(char *domain){
    
    char *dp = domain;
    int indomain_symbs = 0;

    for (int walker = 0; dp[walker] != '\0'; walker++){
        if (walker >= FQDN_MAXLEN){
            return -1;
        }

        if (indomain_symbs == 0){
            if (dp[walker] == '.' || dp[walker] == '-'){
                return -1;
            }
        }

        if (dp[walker] == '.'){
            indomain_symbs = 0;
            continue;
        }


        /* check if domain contains valid symbols */
        if ('9' >= dp[walker] && dp[walker] >= '0' || 'z' >= dp[walker] && dp[walker] >= 'a' ||  
            'Z' >= dp[walker] && dp[walker] >= 'A' ||  dp[walker] == '-'){
                indomain_symbs++;
        } else {
            return -1;
        }
    }
    return 0;
}