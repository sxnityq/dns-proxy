struct master_ipv4{
    union {
        unsigned int ipv4;
        unsigned char octets[4];
    };
};


int check_ipv4(char*, struct master_ipv4 *);
