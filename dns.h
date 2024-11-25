#include <stdint.h>


/*
new dns_headers flags format because i really hard fucked up with struct bit fields 


  CHECK DESCRIPTION BELOW!1!!
                                  1  1  1  1  1  1
    0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|RA|   Z    |   RCODE   |QR|   Opcode  |AA|TC|RD|
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

*/

#define RA      0x8000
#define Z       0x7000
#define RCODE   0xF00
#define QR      0x80
#define OPCODE  0x78
#define AA      0x4
#define TC      0x2
#define RD      0x1


/* 

The header contains the following fields:
                                    1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    QDCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ANCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    NSCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ARCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

REMEMBER THIS IS NETWORK (BIG ENDIAN ORDERING)
SO AFTER CONVERTING THE ORDER TO EJECT WILL BE:

                                  1  1  1  1  1  1
    0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|RA|   Z    |   RCODE   |QR|   Opcode  |AA|TC|RD|
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

 */
struct dns_header2 {
    /* dentifier: A 16-bit identification field generated by the device that creates the DNS query. 
    It is copied by the server into the response, so it can be used by that device 
    to match that query to the corresponding reply received from a DNS server */
    uint16_t ID;
    /* OLD NOT WORKING SHIT
    union generic_flags {
        struct bit_flags {
            uint16_t QR : 1;       // Query/Response 0 on query
            uint16_t opcode : 4;   // detailed shit about query
            uint16_t AA : 1;       // indicate if generated by authority
            uint16_t TC : 1;       // indicate if message is truncated
            uint16_t RD : 1;
            uint16_t RA : 1;        // recursive available
            uint16_t Z : 3;        // shoud be zero
            uint16_t Rcode : 4;    // response code
        } bit_flags;
        
        uint16_t bits;
    } generic_flags; */
    
    uint16_t bits;
    /* Question Count: 
    * Specifies the number of questions in the Question section of the message. 
    */
    uint16_t QDcount;
    /* Answer Record Count:
     * Specifies the number of resource records in the Answer section of the message. 
     */
    uint16_t ANcount;
    /* Authority Record Count: 
     * Specifies the number of resource records in the Authority section of the message. 
     * (“NS” stands for “name server”, of course. J) 
     */
    uint16_t NScount;
    /* Additional Record Count: 
     * Specifies the number of resource records in the Additional section of the message. 
     */
    uint16_t ARcount;
};

/* gets pointer and buffer where to store domain. Returns how many bytes it was listened */
int get_domain(char *p, char *buf, int maxs);

/* 
 *  compairs two domains in register ind/ependent way + ignoring trailing dot 
 *  gOOgle.COM  =   google.com.
 *  googl.com  != google.com.
 *  0 - equal; 1 - false
 */ 
int compair_domains(char *d1, char *d2);