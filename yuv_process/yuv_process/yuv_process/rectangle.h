#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include <stdint.h>
#include <vector>

class Rect
{
public:
    uint32_t left;
    uint32_t top;
    uint32_t right;
    uint32_t bottom;
    inline bool operator == (const Rect &rect1) const;
};

void merge_rect(std::vector<Rect> &rects);

#endif
