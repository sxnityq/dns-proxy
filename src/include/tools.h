#include "common_types.h"

#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define FQDN_MAXLEN     255


int check_ipv4(char *);

/* converts ipv4 in string format like "127.0.0.1" to int */
int src_ipv4to_int(char *);
void int_ipv4to_src(unsigned long, unsigned char *);

/* 
 *  0 - IF VALID
 *  FQDN only with(out) trailing dot  
 *  BOTH ARE VALID:
 *  google.com
 *  google.com.
 */

int check_domain(char *);