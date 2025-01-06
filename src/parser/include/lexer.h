#ifndef LEXER_H
#define LEXER_H

/*
    ID      :=  MASTER
    IPV4    :=  228.228.228.228
    DOMAIN  :=  google.com.
    STRING  :=  "Town, Flow of time, People"

    LB      :=  [ 
    RB      :=  ] 
    COLON   :=  :
    BLANK   :=  <tab> <space>
    NL      :=  <newline>
    TEOI    :=  token for END OF INPUT
    ERR     :=  error token
*/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "io.h"
#include "../../include/colors.h"


#define RXIO_PRINTF(ttype, lexeme) if (ttype == ERR){                                              \
            printf("%s<%6s> %s %s%s\n", RED, get_human_ttype(ttype), "=", lexeme, ENDCOLOR);      \
        } else if (ttype == TEOI) {                                                                \
            printf("%s<%6s> %s %s%s\n", BLUE, get_human_ttype(ttype), "=", lexeme, ENDCOLOR);     \
        } else {                                                                                   \
            printf("%s<%6s> %s %s%s\n", GREEN, get_human_ttype(ttype), "=", lexeme, ENDCOLOR);    \
        }

#define ERR_PRINTF(ttype, lexeme, reason) printf("%s%s < %s > = < %s > AT LINE %d%s\n", \
        RED, reason, get_human_ttype(ttype), lexeme, get_lineno(), ENDCOLOR)

#define INVALID_TOKEN(ttype) printf("%sINVALID TOKEN: <%s>  COLUMN: %d LINE: %d %s\n", RED, \
    get_human_ttype(ttype), get_colno(), get_lineno(), ENDCOLOR);


#define get_ttypes_count() NUMBER_OF_TYPES

#define DEBUG   0
#define MAX_DOMAIN_LEN  255

#define T_ERROR     0       /* return if error occured during token recognition */
#define T_OK        1       /* return if token was successfully recognised */
#define T_OVERFLOW  10      /* return if overflow was detected (to long lexeme) */

#define LB_STATE        1
#define RB_STATE        2
#define COLON_STATE     3
#define IPV4_STATE      4
#define STRING_STATE    5
#define ID_STATE        6
#define DOMAIN_STATE    7
#define NL_STATE        8
#define EOI_STATE       9
#define ERR_STATE       10


enum TTYPES {
    ID,
    IPV4,
    DOMAIN, 
    STRING,
    LB,
    RB,
    COLON,
    BLANK,
    NL,
    TEOI,
    ERR,
    UNINIT,
    NUMBER_OF_TYPES
};

extern unsigned char *human_ttypes[];

typedef struct {
    int ttype;
    int tlen;
    unsigned char *lexeme;
} Token;

extern Token ctoken;
extern Token ptoken;



char *get_human_ttype(int);

/* return true if next char is independent token  */
int is_independent(char);


/* return true if next char is independent token  */
void move_prev_token();

void retract_token();


void create_token(int);

/* used  to advance until independent token without modifying lexeme len*/
void strip_io();

/* iterates through input buffer until it finds independent tokens and marks invalid token as error token */
void look_error();
/* Look for valid ipv4 token */
int look_ipv4();
int look_string();
int look_id();
int look_domain();
void look_blank();

void get_token();
void list_tokens();


#endif  // LEXER_H