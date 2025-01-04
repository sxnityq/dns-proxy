#include "include/colors.h"
#include "include/cfg_types.h"
#include "include/llist.h"
#include "include/args.h"
#include "include/dns.h"
#include "include/tools.h"

#include "parser/include/io.h"
#include "parser/include/parser.h"


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define IN      1
#define OUT     0

#define PORT            53
#define PROT            "UDP"
#define READBUFFLEN     512


Master          Master_arg;
BlackList       BlackList_arg;
ResponseType    Respose_arg;


struct sockaddr_in bind_addrinfo;
int sockfd;



/* on success return socket file descriptor */
int init_connection(void);
void event_loop(void);

/*  check if domain is blacklisted 
 *  1 - IS blacklisted
 *  0 - IS NOT blacklisted
 */
int is_blacklisted(char *d1);


/* 

The configuration file contains the following parameters:
    ◦ IP address of upstream DNS server
    ◦ List of domain names to filter resolving ("blacklist")
    ◦ Type of DNS proxy server's response for blacklisted domains 
(not found, refused, resolve to a pre-configured IP address)

*/


int build_bl_response(char *buf, struct dns_header2 *og_header){
    struct dns_header2 header;

    // REFUSED is 5. SO RCODE = 0101

    unsigned short int rc_flg = 5; //0x101
    unsigned short int bits = 0xFFFF;

    bits  = ((bits << 4) & og_header->bits) & rc_flg;   // set RCODE to 5 (refused)
    bits = bits | 0x8000;                               // masrk as a response
    header.bits = htons(bits);
    header.ANcount = 0;
    header.ARcount = 0;
    header.NScount = 0;
    header.QDcount = 0;
    header.ID = htons(og_header->ID);
    memcpy(buf, &header, sizeof(struct dns_header2));

    return sizeof(struct dns_header2);

}


int init_connection(void){

    memset(&bind_addrinfo, '\0', sizeof(struct sockaddr_in));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        printf("socket error\n");
        return -1;
    }

    bind_addrinfo.sin_port = htons(PORT);
    bind_addrinfo.sin_family = AF_INET;
    bind_addrinfo.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&bind_addrinfo, sizeof(struct sockaddr_in)) < 0){
        printf("bind error\n");
        close(sockfd);
        return -1;
    }

    printf("Server is running on port 53...\n");

    return sockfd;
}


void event_loop(void){

    struct sockaddr_in their_sockaddr, fill_sa, rcv_sa;
    socklen_t their_addr_size, rcv_sa_size, send_sockaddr_size;
    char r_buff[READBUFFLEN], rcv_buff[READBUFFLEN];
    int numbytes, sendbytes, rcvbytes, bl_response;
    struct dns_header2 header, rcv_header;
    
    rcv_sa_size = sizeof(struct sockaddr_in);
    their_addr_size = sizeof(struct sockaddr_in);
    send_sockaddr_size = rcv_sa_size;

    struct sockaddr_in upstream_sa;
    char domain2upstream[FQDN_MAXLEN];
    uint16_t res;


    char ipv4[32];
    char bl_buff[READBUFFLEN];


    upstream_sa.sin_port = htons(PORT);
    upstream_sa.sin_family = AF_INET;
    upstream_sa.sin_addr.s_addr = htonl(Master_arg.ipv4.ipv4);

    while(1){
        memset(r_buff, '\0', READBUFFLEN);
        memset(ipv4, '\0', 32);

        if ( (numbytes = recvfrom(sockfd, r_buff, READBUFFLEN - 1, 0,
         (struct sockaddr *)&fill_sa, &rcv_sa_size))  == -1){
            printf("recvfrom error\n");
            exit(-1);
        }

        if (numbytes < 12){
            printf("is it really dns query/answer?\n");
            exit(-1);
        }

        
        r_buff[numbytes] = '\0';
        memcpy(&header, r_buff, sizeof(struct dns_header2));
        memset(domain2upstream, '\0', FQDN_MAXLEN);
        
        get_domain((r_buff + sizeof(struct dns_header2)), domain2upstream, FQDN_MAXLEN);

        header.QDcount = ntohs(header.QDcount);
        header.ID = ntohs(header.ID);
        header.ANcount = ntohs(header.ANcount);
        header.NScount = ntohs(header.NScount);
        header.ARcount = ntohs(header.ARcount);
        header.bits = ntohs(header.bits);
        
        if (is_blacklisted(domain2upstream) != 0){
            memset(bl_buff, '\0', READBUFFLEN);
            bl_response =  build_bl_response(bl_buff, &header);
            sendto(sockfd, bl_buff, bl_response, 0, (struct sockaddr *)&fill_sa, sizeof(struct sockaddr));
            printf("BLACKLISTED. GIVE IT BACK\n");
            continue;
        }
        
        printf("=======================\n"
               "+ RCV BYTES: %d +\n"
               "=======================\n", numbytes);
        
        printf("***** RCV FLAGS *****\n"
        "QR: %d OP: %d AA: %d TC: %d\n" 
        "RD: %d RA: %d Z:  %d RC: %d\n",
        (header.bits & QR) >> 15, (header.bits & OPCODE) >> 11,
        (header.bits & AA) >> 10, (header.bits & TC) >> 9,
        (header.bits & RD) >> 8, (header.bits & RA) >> 7,
        (header.bits & Z) >> 4, header.bits & RCODE);
        
        printf("***** ID: %i *****\n", header.ID);
        printf("+ domain: %s\n", domain2upstream);
        printf("+ sender ip: %s\n", inet_ntop(AF_INET,  &(fill_sa.sin_addr), ipv4, sizeof(struct sockaddr)));
        printf("+ sender port: %i\n", ntohs(fill_sa.sin_port));

        res = manage_queue_by_existence(header.ID, (struct sockaddr *)&fill_sa, 
                (struct sockaddr *)&rcv_sa);

        if (res == 0){
            printf("***** REQUEST... *****\n");
            sendbytes = sendto(sockfd, r_buff, numbytes, 0, (struct sockaddr *)&upstream_sa,
             send_sockaddr_size);
        } else {
            printf("***** RESPONSE FOR: %d *****\n", res);
            printf("+ send back to: %s\n", inet_ntop(AF_INET,  
                &(rcv_sa.sin_addr), ipv4, sizeof(struct sockaddr)));
            printf("+ send back to: %i\n", ntohs(rcv_sa.sin_port));
            
            sendbytes = sendto(sockfd, r_buff, numbytes, 0, (struct sockaddr *)&rcv_sa,
                rcv_sa_size);
        }
        printf("========================\n\n");
    }
    close(sockfd);
}


int get_domain(char *p, char *buf, int maxs){

    int walker = 0;
    int scan;
    while (*p != '\0'){
        scan = *p;
        *p++;
        for (int j = 0; j < scan; j++, p++){
            buf[walker] = *p;
            walker++;
            if (walker >= maxs){
                return -1;
            }
        }
        buf[walker] = '.';
        walker++;
    }
    walker++;
    buf[walker] = '\0'; //to allow domain comparison
    return walker;
}


int compair_domains(char *d1, char *d2){
    int walker = 0;
    while (1){

        if (d1[walker] == '\0'){
            if (d2[walker] == '\0' || d2[walker] == '.' && d2[walker + 1] == '\0'){
                return 0;
            }
            return 1;
        }

        if (d2[walker] == '\0'){
            if (d1[walker] == '\0' || d1[walker] == '.' && d1[walker + 1] == '\0'){
                return 0;
            }
            return 1;
        }

        if (tolower(d1[walker]) != tolower(d2[walker])){
            return 1;
        }
        walker++;
    }
}


int is_blacklisted(char *d1){
    BlackList *p;

    for (p = &BlackList_arg; p->next != NULL; p = p->next){
        if (compair_domains(d1, p->domain.domain_lexeme) == 0){
            return 1; //is blacklisted
        }
    }
    return 0;   //is not blacklisted
}


int main(int argc, char *argv[]){    

    inpfile = open_newfile("config.txt");
    
    main_parser();
    
    init_q_head();      // queue_head = NULL;
    sockfd = init_connection();
    if (sockfd < 0){
        exit(1);
    }

    event_loop();
}