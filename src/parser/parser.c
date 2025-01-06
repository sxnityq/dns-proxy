#include "include/lexer.h"
#include "include/parser.h"
#include "../include/args.h"
#include "../include/tools.h"

#include <stdio.h>

/* 
    STATEMENT ::= 
            master colon ipv4               |
            ban colon lb DOMAINS_LIST rb    |
            type colon string

    DOMAINS_LIST ::= domain DOMAINS_LIST | epsilon

*/

int was_parsing_error = 0;


static void move_next_statement()
{
    while (ctoken.ttype != NL && ctoken.ttype != TEOI){
        get_token();
    }
}


static int syntes_master(Token ipv4_t)
{
    if (ipv4_t.ttype != IPV4){
        return PARSE_ERR;
    }

    if (check_ipv4(ipv4_t.lexeme)){
        Master_arg.ipv4.ipv4 = src_ipv4to_int(ipv4_t.lexeme);
    }

    #if DEBUG
        printf("master ipv4: %d\n", Master_arg.ipv4.ipv4);
    #endif

    return PARSE_OK;
}


static int parse_master()
{
    get_token();

    if (ctoken.ttype == COLON){
        get_token();
        if (ctoken.ttype == IPV4 ){
            syntes_master(ctoken);
            get_token();
            if (ctoken.ttype == NL || ctoken.ttype == TEOI){
                return PARSE_OK;
            } else {
                ERR_PRINTF(ctoken.ttype, ctoken.lexeme, "UNEXPECTED TOKEN. LOOKING FOR <NL> OR <EOI>. GIVED: ");
            }
        } else {
            ERR_PRINTF(ctoken.ttype, ctoken.lexeme, "UNEXPECTED TOKEN. LOOKING FOR <IPV4>. GIVED: ");
        }
    } else {
        ERR_PRINTF(ctoken.ttype, ctoken.lexeme, "UNEXPECTED TOKEN. LOOKING FOR <COLON>. GIVED: ");
    }

    was_parsing_error = 1;
    return PARSE_ERR;
}

static int parse_domain_list()
{
    int chunks;
    BlackList *next;

    get_token();
    next = &BlackList_arg;

    while (ctoken.ttype == DOMAIN){
        next->domain.domain_lexeme = calloc(ctoken.tlen, 1);
        strncpy(next->domain.domain_lexeme, ctoken.lexeme, ctoken.tlen);
        next->domain.domain_size = ctoken.tlen;
        next->next = calloc(sizeof(BlackList), 1);
        next = next->next;
        
        #if DEBUG
            printf("added domain: %s\n", ctoken.lexeme);
        #endif

        get_token();
    }

    next->next = NULL;
    
/*     retract_token();
    discard_lexeme(); */
}

static int clear_Ban_arg()
{
    BlackList *p;    

    p = &BlackList_arg;

    if (p->next != NULL){
        free(p->domain.domain_lexeme);
        p = p->next;
    }

    for (; p->next != NULL; p = p->next){
        free(p->domain.domain_lexeme);
        free(p);
    }

    BlackList_arg.next = NULL;
}

static int parse_ban()
{
    get_token();

    if (ctoken.ttype == COLON){
        get_token();
        if (ctoken.ttype == LB){
            parse_domain_list();
            /*  get_token(); */
            if (ctoken.ttype == RB){
                get_token();
                if (ctoken.ttype == NL || ctoken.ttype == TEOI ){
                    #if DEBUG 
                        printf("BanList parsed!\n");
                    #endif
                    return PARSE_OK;
                } else {
                    ERR_PRINTF(ctoken.ttype, ctoken.lexeme, "UNEXPECTED TOKEN. LOOKING FOR <NL> OR <EOI>. GIVED: ");
                }
            } else {
                ERR_PRINTF(ctoken.ttype, ctoken.lexeme, "UNEXPECTED TOKEN. LOOKING FOR <RB>. GIVED: ");
            }
        } else {
            ERR_PRINTF(ctoken.ttype, ctoken.lexeme, "UNEXPECTED TOKEN. LOOKING FOR <LB>. GIVED: ");
        }
    }   else {
        ERR_PRINTF(ctoken.ttype, ctoken.lexeme, "UNEXPECTED TOKEN. LOOKING FOR <COLON>. GIVED: ");
    }
    
    was_parsing_error = 1;
    return PARSE_ERR;
}


static int parse_id(Token token)
{ 
    if (!strncmp(token.lexeme, "MASTER", token.tlen)){
        return parse_master();
    }

    if (!strncmp(token.lexeme, "BAN", token.tlen)){
        if (!(BlackList_arg.next == NULL && BlackList_arg.domain.domain_size == 0)){
            clear_Ban_arg();
        }
        return parse_ban();
    }

    was_parsing_error = 1;
    ERR_PRINTF(token.ttype, token.lexeme, "INVALID KEYWORD");
}

static int parse_stmt()
{
    get_token();
    if (ctoken.ttype  == ID){
        parse_id(ctoken);
    } else if (ctoken.ttype == NL || ctoken.ttype == TEOI) {
        return PARSE_OK;
    } else {
        ERR_PRINTF(ctoken.ttype, ctoken.lexeme, "shoud be ID, got:");
        was_parsing_error = 1;
    }
    move_next_statement();
}

int main_parser()
{
    while(ctoken.ttype != TEOI){
        parse_stmt();
    }
    
    if (!was_parsing_error){
        unsigned char s[64];
        int_ipv4to_src(Master_arg.ipv4.ipv4, s);
        printf("CONFIG SUCCESSFULLY PARSED\n"
               "MASTER: %s\n", s);
        
        BlackList *p;
        for (p = &BlackList_arg; p->next != NULL; p = p->next){
            printf("BAN: %s\tlen: %d\n", p->domain.domain_lexeme, p->domain.domain_size);
        }
    }

    return was_parsing_error;
}