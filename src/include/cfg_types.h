#ifndef CONFIG_TYPES_DEFINED_H
#define CONFIG_TYPES_DEFINED_H

#include "common_types.h"

typedef struct{
    union Ipv4 ipv4;
} Master;


typedef struct BlackList{
    struct Domain domain;
    struct BlackList *next;
} BlackList;

typedef struct {
    char *str;
} ResponseType;

#endif