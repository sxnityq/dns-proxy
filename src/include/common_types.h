#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

union Ipv4 {
    unsigned long ipv4;
    unsigned char octets[4];
};

struct Domain {
    char *domain_lexeme;
    int domain_size;
};

#endif  // COMMON_TYPES_H