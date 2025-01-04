#include <stdint.h>
#include <sys/socket.h>


struct domain_q{
    struct sockaddr sa;
    uint16_t        datagram_id;
    struct domain_q *next;
};


void init_q_head(void);
void add2q(uint16_t, struct sockaddr *);
void rmv2q(uint16_t);

/* 
 *  if id DOES NOT exist - add it to tail and return 0 
 *  if id DOES exist - remove it, rearange queue and return remove id 
 *
 *  ARGUMENTS:
 *  sa -> sockaddr struct to add 
 *  rcv_sa -> sockaddr struct that will be filled with removed node sockaddr
 */

uint16_t manage_queue_by_existence(uint16_t, struct sockaddr *, struct sockaddr *);