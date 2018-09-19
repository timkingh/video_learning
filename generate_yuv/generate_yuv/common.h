#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

typedef struct BufInfo {
    char *buf;
    uint32_t width;
    uint32_t height;
} BufInfo;


int64_t time_usec(void);





#endif
