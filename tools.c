#include "tools.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BITS_IN_OCTET   3
#define FQDN_MAXLEN     255


int check_ipv4(char *str_ipv4, struct master_ipv4 *m_ipv4){
    char *p;
    int dec_octet_index = 0;
    int octet = 0;
    int atoi_res;

    char octet_addr_hdlr[BITS_IN_OCTET];

    struct master_ipv4 addr_hdlr;
    memset(&addr_hdlr, '\0', sizeof(struct master_ipv4));


    for (p = str_ipv4; *p != '\0'; p++){
        if (octet > 4){
            return -1;
        }
        
        if (*p == '.'){
            /* for cases like 228..2.2.2 or .2.2.2.2 etc */
            if (dec_octet_index == 0){
                return -1;
            }

            atoi_res = atoi(octet_addr_hdlr);
            
            if (atoi_res > 255){
                return -1;
            }

            addr_hdlr.octets[octet] = atoi_res;
            
            dec_octet_index = 0;
            memset(octet_addr_hdlr, '\0', BITS_IN_OCTET);
            octet++;
            continue;
        }

        /* remember we start at 0. So 2 wil means 3 chars (like 123) */
        if (dec_octet_index == 3){ 
            return -1;
        }

        /* remember we check above for leading dot */
        if (*p >= '0' && *p <= '9'){
            octet_addr_hdlr[dec_octet_index] = *p;
        } else {
            return -1;
        }
        dec_octet_index++;
    }

    /* 
     * for cases like 127.0.0. 10.0.0 etc. 
     * Octet should be 3 cause we dont inc it because we were breaked out from cycle
     */
    
    if (octet != 3){
        return -1;
    }

    /* because we break cycle if *p eq to '\0' */
    atoi_res = atoi(octet_addr_hdlr);
    if (atoi_res > 255){
        return -1;
    }

    addr_hdlr.octets[octet] = atoi_res;

    /* actual copy to master ipv4 struct */
    memcpy(m_ipv4, &addr_hdlr, sizeof(struct master_ipv4));
    return 0;
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