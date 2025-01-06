#ifndef IO_H
#define IO_H

#define BUFFER_SIZE     (1024 + DANGER_ZLEN)
#define MAXLEX          255
#define EOI             0       /* END OF INPUT */
#define OOM             255     /* when clexeme len equals to MAXLEX */
#define DANGER_ZLEN     2
#define DANGER          (END - DANGER_ZLEN)

extern int was_eoi;
extern int inpfile;


unsigned char *get_clexeme();
unsigned char *get_plexeme();
unsigned char *get_forward();

int get_clexeme_len();
int get_plexeme_len();

void move_lexeme();
void move_prev_lexeme();

void buffer_init();
int rxio_fill_buffer(unsigned char *, int);
int rxio_flush();

unsigned char advance(int);
int retract();
int retract_nchars(int);
void retract_lexeme();
void discard_lexeme();

unsigned int get_lineno();
unsigned int get_colno();

int was_eoi_f();
int open_newfile(char *);


#endif  // IO_H