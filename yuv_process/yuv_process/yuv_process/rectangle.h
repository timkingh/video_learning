#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include <stdint.h>

typedef struct {
    uint32_t left;
    uint32_t top;
    uint32_t right;
    uint32_t bottom;
} Rect;

#endif
