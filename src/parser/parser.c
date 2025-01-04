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

    printf("lexeme: %s\n", ipv4_t.lexeme);
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
    return PARSE_ERR;
}


static int parse_domain_list()
{
    int chunks;
    BlackList *next;

    get_token();
    next = &BlackList_arg;

    while (ctoken.ttype == DOMAIN){
        next->domain.domain_lexeme = malloc(ctoken.tlen);
        strncpy(next->domain.domain_lexeme, ctoken.lexeme, ctoken.tlen);
        next->domain.domain_size = ctoken.tlen;
        next->next = malloc(sizeof(BlackList));
        next = next->next;
        
        #if DEBUG
            printf("added domain: %s\n", ctoken.lexeme);
        #endif

        get_token();
    }

    next->next = NULL;
    
    retract_token();
    discard_lexeme();
}


static int parse_ban()
{
    get_token();

    if (ctoken.ttype == COLON){
        get_token();
        if (ctoken.ttype == LB){
            parse_domain_list();
            get_token();
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
    return PARSE_ERR;
}


static int parse_id(Token token)
{ 
    if (!strncmp(token.lexeme, "MASTER", token.tlen)){
        return parse_master();
    }

    if (!strncmp(token.lexeme, "BAN", token.tlen)){
        return parse_ban();
    }

    ERR_PRINTF(token.ttype, token.lexeme, "INVALID KEYWORD");
}

static int parse_stmt()
{
    get_token();
    if (ctoken.ttype  == ID){
        parse_id(ctoken);
        move_next_statement();
    } else if (ctoken.ttype == NL || ctoken.ttype == TEOI) {
        return PARSE_OK;
    } else {
        ERR_PRINTF(ctoken.ttype, ctoken.lexeme, "shoud be ID, got:");
        return PARSE_ERR;
    }
}

int main_parser()
{
    while(ctoken.ttype != TEOI){
        parse_stmt();
    }
}

/* int main(int argc, char *argv[])
{
    inpfile = open_newfile("config.txt");
    main_parser();
    return 0;
} */