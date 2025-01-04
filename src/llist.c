#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "include/llist.h"

struct domain_q *queue_head;

void init_q_head(void){
    queue_head = NULL;
};

void add2q(uint16_t new_datagram_id, struct sockaddr *sa){
    if(queue_head == NULL){
        queue_head->datagram_id = new_datagram_id;
        queue_head->next = NULL;
        memcpy(&(queue_head->sa), sa, sizeof(struct sockaddr));
        return;
    }
    
    struct domain_q     *p;

    for(p = queue_head; p->next != NULL; p=p->next){
        ;
    }
    
    p->next = (struct domain_q *) malloc(sizeof(struct domain_q));
    p->next->next = NULL;
    p->next->datagram_id = new_datagram_id;
    memcpy(&(p->next->sa), sa, sizeof(struct sockaddr));
    return;
};

void rmv2q(uint16_t rmv_datagram_id){
    if(queue_head == NULL){
        return;
    }
    struct domain_q *p;
    struct domain_q *freep;

    p = queue_head;

    if (queue_head->datagram_id == rmv_datagram_id){
        queue_head = queue_head->next;
        free(p);
        return;
    }

    for(p = queue_head; p->next != NULL; p=p->next){
        if(p->next->datagram_id == rmv_datagram_id){
            freep = p->next;
            p->next = p->next->next;
            free(freep);
            return;
        }
    }
    return;
}

/* 
 *  if id DOES NOT exist - add it to tail and return 0 
 *  if id DOES exist - remove it, rearange queue and return remove id 
 *
 *  ARGUMENTS:
 *  sa -> sockaddr struct to add 
 *  rcv_sa -> sockaddr struct that will be filled with removed node sockaddr
 */

uint16_t manage_queue_by_existence(uint16_t check_datagram_id, 
    struct sockaddr *sa, struct sockaddr *rcv_sa){
    
    if(queue_head == NULL){
        queue_head =  (struct domain_q *) malloc(sizeof(struct domain_q));
        queue_head->datagram_id = check_datagram_id;
        queue_head->next = NULL;
        memcpy(&(queue_head->sa), sa, sizeof(struct sockaddr));
        return 0;
    }
    
    struct domain_q *p;
    struct domain_q *freep;

    p = queue_head;

    if (queue_head->datagram_id == check_datagram_id){
        memcpy(rcv_sa, &(queue_head->sa), sizeof(struct sockaddr));
        queue_head = queue_head->next;
        free(p);
        return check_datagram_id;
    }

    for(p = queue_head; p->next != NULL; p=p->next){
        if(p->next->datagram_id == check_datagram_id){
            memcpy(rcv_sa, &(p->next->sa), sizeof(struct sockaddr));
            freep = p->next;
            p->next = p->next->next;
            free(freep);
            return check_datagram_id;
        }
    }
    
    p->next = (struct domain_q *) malloc(sizeof(struct domain_q));
    p->next->next = NULL;
    p->next->datagram_id = check_datagram_id;
    memcpy(&(p->next->sa), sa, sizeof(struct sockaddr));
    return 0;
}