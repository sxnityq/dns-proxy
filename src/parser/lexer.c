#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "include/io.h"
#include "include/lexer.h"


unsigned char *human_ttypes[] = {
    "ID",
    "IPV4",
    "DOMAIN",
    "STRING",
    "LB",
    "RB",
    "COLON",
    "BLANK",
    "NEW LINE",
    "EOI",
    "ERROR",
    "UNINITIALISED"
};

Token ctoken = {
    .ttype  = UNINIT,
    .tlen   = 0,
    .lexeme = NULL,
};

Token ptoken = {
    .ttype  = UNINIT,
    .tlen   = 0,
    .lexeme = NULL,
};

char *get_human_ttype(int ttype)
{
    if (ttype > get_ttypes_count()){
        return human_ttypes[ERR];
    }
    return human_ttypes[ttype];
}

int is_independent(char sym)
{   
    return (
        sym == '['  || sym == ']'  || sym == ':' || 
        isblank(sym)|| sym == '\n' || was_eoi
    );
}


void move_prev_token()
{
    free(ptoken.lexeme);
    ptoken.lexeme   = ctoken.lexeme;
    ptoken.tlen     = ctoken.tlen;
    ptoken.ttype    = ctoken.ttype;

}

void retract_token()
{
    if (ctoken.lexeme != ptoken.lexeme){
        free(ctoken.lexeme);
        ctoken.lexeme   = ptoken.lexeme;
        ctoken.tlen     = ptoken.tlen;
        ctoken.ttype    = ptoken.ttype;
    }
}


void create_token(int ttype)
{
    ctoken.tlen = get_clexeme_len();
    ctoken.ttype = ttype;
    ctoken.lexeme = calloc(get_clexeme_len(), 1);
    memcpy(ctoken.lexeme, get_clexeme(), get_clexeme_len());

    
    #if DEBUG
        RXIO_PRINTF(ctoken.ttype, ctoken.lexeme);
    #endif
    

    if (get_clexeme() != get_plexeme()){
        free(ptoken.lexeme);
        move_prev_lexeme();
    }
    move_lexeme();
}

/* used  to advance until independent token without modifying lexeme len*/
void strip_io()
{
    unsigned char c;
    while(1){
        c = advance(1);
        if (is_independent(c)){
            retract();
            return;
        }
    }
}

/* iterates through input buffer until it finds independent tokens and marks invalid token as error token */
void look_error()
{
    unsigned char c;

    while (1){
        c = advance(0);
        if (is_independent(c)){
            return;
        }
    }
}


/* Look for valid ipv4 token */
int look_ipv4()
{
    unsigned char c;

    int listed_octets, walker;
    int o1 = 0, o2 = 0, o3 = 0;

    listed_octets = 1;
    
    c = advance(0);

    while(listed_octets <= 4){
        for (walker = 1; walker < 4; walker++){
            if (!isdigit(c)){
                break;
            }
            if (walker == 1){
                o1 = c - '0';
            } else if (walker == 2){
                o2 = c - '0';
            } else {
                o3 = c - '0';
            }
            c = advance(0);
        }

        if (walker == 1){
            return T_ERROR;
        }

        if (walker == 3){
            o1 *= 10;
        }

        if (walker == 4){
            o1 *= 100;
            o2 *= 10;
        } 

        if (listed_octets < 4){
            if (c != '.'){
                return T_ERROR;
            }
        }

        if ( (o1 + o2 + o3) > 255){
            return T_ERROR;
        
        }

        if (listed_octets == 4){
            if (is_independent(c)){
                return T_OK;
            }
        }

        o1 = 0, o2 = 0, o3 = 0;
        listed_octets++;
        c = advance(0);
    }

    return T_ERROR;
}

/* look for string token */
int look_string()
{
    unsigned char c;

    while (1){
        c = advance(0);

        if (c == EOI){
            return T_ERROR;
        }

        if (c == '"'){
            return T_OK;
        }
    }
}


int look_id()
{
    unsigned char c;

    while(1){
        c = advance(0);

        if (is_independent(c)){
            return T_OK;         
        }

        if (ispunct(c)){
            return T_ERROR;
        }
    }
}


int look_domain()
{
    char c;

    int indomain_symbs = 0;
    int walker = 1;

    c = advance(0);

    while(1){

        if (is_independent(c)){
            return T_OK;
        }

        if (walker >= MAX_DOMAIN_LEN){
            return T_ERROR;
        }

        if (indomain_symbs == 0){
            if (c == '.' || c == '-'){
                return T_ERROR;
            }
        }

        /* check if domain contains valid symbols */
        if ('9' >= c && c >= '0' || 'z' >= c && c >= 'a' ||  
            'Z' >= c && c >= 'A' ||  c == '-'){
                indomain_symbs++;
            }
        else if (c == '.'){
            indomain_symbs = 0;
        } else {
            return T_ERROR;
        }

        c = advance(0);
        walker++;
    }
    return T_ERROR;
}


void look_blank()
{
    unsigned char c;
    while (1){
        c = advance(0);
        if (!isblank(c)){
            return;
        }
    }
}


/* forward until reach one of the "accept states" */
void get_token()
{       
    int state;
    int look_result;
    unsigned char c;

    state       = 0;

    while(1){
        if (was_eoi) {
            return create_token(TEOI);
        }

        switch (state)
        {
        case 0:
            c = advance(0);

            if (c == '['){
                state = 1;
                break;
            }
            if (c == ']'){
                state = 2;
                break;
            }
            if (c == ':'){
                state = 3;
               
                break;
            }
            if (isdigit(c)){
                state = 4;
                break;
            }
            if (c == '"'){
                state = 5;
                break;
            }
            if (isalpha(c)){
                state = 6;
                break;
            }

            if (isblank(c)){
                state = 10;
                break;
            }

            if (c == '\n'){
                state = 11;
                break;
            }

            if (c == EOI){
                state = 12;
                break;
            }

            state = 99;
            break;
        
        /* for left brace [ */
        case 1:
            return create_token(LB);
        
        /* for right brace ] */
        case 2:
            return create_token(RB);

        /* for colon : */
        case 3:
            return create_token(COLON);
        
        /* for ipv4 */   
       case 4:
            retract();
            if (!look_ipv4()){
                retract();
                INVALID_TOKEN(IPV4);
                look_error();
                retract();

                return create_token(ERR);
            }
            retract();
            return create_token(IPV4);
       
       /* for strings */
        case 5:
            look_result = look_string();
            
            if (look_result ==  T_OK){
                return create_token(STRING);
            }

            if (look_result == T_ERROR){
                INVALID_TOKEN(STRING);
                retract();
                look_error();
                return create_token(ERR);
            }

        /*  look for identifiers. If no match switch to lookinf for domains */
        case 6:
            retract();
            if (!look_id()){
                retract_lexeme();
                state = 7;
                break; 
            }
            retract();
            return create_token(ID);            


        /* look for domain */
        case 7:
            if (!look_domain()){
                INVALID_TOKEN(DOMAIN);
                retract();
                look_error();
                retract();
                return create_token(ERR);
            }

            retract();
            return create_token(DOMAIN);

        case 10:
            retract();
            look_blank();
            retract();
            create_token(BLANK);

            /* because we strip whitespaces */
            return get_token();

        /* for new line token */
        case 11:
            return create_token(NL);
        
        case 12:
            return create_token(TEOI);

        case 99:
            look_error();
            retract();
            return create_token(ERR);
        }
    }
}


void list_tokens(){
    get_token();
    while (ctoken.ttype != TEOI && was_eoi == 0){
        RXIO_PRINTF(ctoken.ttype, ctoken.lexeme);
        move_lexeme();
        get_token();
    }
}


/* int main(int argc, char *argv[])
{
    inpfile = open_newfile("config.txt");
    list_tokens();
    return 0;
} */