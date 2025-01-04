#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "include/io.h"

#define END             (&buffer[BUFFER_SIZE])
#define START           (&buffer[0])
#define NO_MORE_CHARS() (was_eoi || *forward == EOI && forward < DANGER)


static unsigned int lineno  = 1;

/* how much lines occupies current lexeme. It is needed when playing with retracted */
static unsigned int clexeme_lines   = 0;
static unsigned char buffer[BUFFER_SIZE];
static unsigned char *clexeme;
static unsigned char *forward;
static unsigned char *plexeme;

static int clexeme_len  = 0;
static int plexeme_len  = 0;

int was_eoi = 0;
int inpfile = 0;


unsigned char *get_clexeme()
{
    return clexeme;
}

unsigned int get_lineno()
{
    return lineno;
}


unsigned char *get_plexeme()
{
    return plexeme;
}



int get_clexeme_len()
{
    return clexeme_len;
}

int get_plexeme_len()
{
    return plexeme_len;
}


void move_lexeme()
{
    /* it points on the beginning of current token (first char) */
    clexeme = forward + 1;
    clexeme_len  = 0;
    forward++;
}

void move_prev_lexeme()
{
    plexeme = clexeme;
    plexeme_len = clexeme_len;
}

int was_eoi_f()
{
    return was_eoi ? 1 : 0;
}


int rxio_fill_buffer(unsigned char *fill_position, int fill_amt)
{
    int readc;
    readc = read(inpfile, fill_position, fill_amt);
    
    if (readc == 0){
        return readc;
    }

    if (readc < fill_amt){
        printf("%s - %d\n", "Perhaps EOI detected!", readc);
        fill_position[readc] = EOI;
    }

    return readc;
}


int rxio_flush()
{
    unsigned char *left_edge;
    int shift_amt, fill_amt;

     /* .....P.....C....D. 
        from D to P is amount of data to shift
        P.....C....D......
    */

    left_edge = plexeme;
    shift_amt = DANGER - left_edge;
    fill_amt  = left_edge - START;
    memcpy(START, left_edge, shift_amt);
    rxio_fill_buffer(START + shift_amt, fill_amt);

    clexeme -= fill_amt;
    forward -= fill_amt;
    plexeme -= fill_amt;
    return 0;
}


void buffer_init()
{
    /* 
     *  cleans buffers and set last char to eof
     */

    static int init_been_called = 0;

    if (!init_been_called){
        memset(buffer, EOI, BUFFER_SIZE);
        clexeme = DANGER;
        forward = DANGER;
        plexeme = DANGER;
        rxio_flush();
    }
}


unsigned char advance(int is_oom)
{
    
    static int been_called = 0;

    if (clexeme_len >= MAXLEX){

        if (!is_oom){
            return OOM;
        }
    } else {
        clexeme_len++;
    }
    
    if (!been_called){    
        buffer_init();
        been_called = 1;
        return *forward;
    }

    if (clexeme_len == 1){
        if (NO_MORE_CHARS()){
            was_eoi = 1;
        }
       return *forward;
    }

    if (++forward >= DANGER){
        rxio_flush();
    }

    if (*forward == '\n'){
        lineno++, clexeme_lines++;
    }
    
    if (NO_MORE_CHARS()){
        was_eoi = 1;
    }

    return *forward;
}


int retract()
{
    if (*forward == '\n'){
        clexeme_lines--, lineno--;
    }


    if (clexeme_len >= 1){
        if (forward > clexeme){
            --forward;
        }
        clexeme_len--;
    }

    return *forward;
}


/* discard all forwarding */
void retract_lexeme()
{
    clexeme_len = 0;
    forward = clexeme;
}

/* repoint clexeme to beginning of previous lexeme */
void discard_lexeme()
{
    /* MASTER -> clexeme M | forward R */
    if (clexeme == plexeme){
        return;
    }

    clexeme = plexeme;
    forward = clexeme;
    clexeme_len = 0;
}

int retract_nchars(int count)
{
    while(--count > 0){
        if (forward > clexeme){
            forward--;
        } else {
            break;
        }
    }

    return *forward;
}


int open_newfile(char *path)
{
    int res;
    res = open(path, O_RDONLY);
    
    if (res == -1){
        return res;
    }

    if (inpfile != 0){
        close(inpfile);
    }

    inpfile = res;
    return inpfile;
}
