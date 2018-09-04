#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include <stdint.h>
#include <vector>

typedef struct {
    char *buf;
    unsigned int width;
    unsigned int height;
} YuvInfo;

typedef struct {
    uint32_t left;
    uint32_t top;
    uint32_t right;
    uint32_t bottom;
    uint8_t  y_pixel;
    uint8_t  u_pixel;
    uint8_t  v_pixel;
} RectangleInfo;

class Rect
{
public:
    uint32_t left;
    uint32_t top;
    uint32_t right;
    uint32_t bottom;
    uint32_t motion_rate;
    uint32_t distance;
    uint32_t area;

    inline bool operator == (const Rect &rect1) const;
    void calculate_area();
};

void merge_rect(void *proc_ctx, std::vector<Rect> &rects, std::vector<Rect> &rects_org);
void merge_rect_optimize(void *proc_ctx, std::vector<Rect> &rects_org, std::vector<Rect> &rects_new);
void draw_blue_rectangle(YuvInfo *yuv, std::vector<Rect> &rects);
void draw_rectangle(YuvInfo *yuv, RectangleInfo *rec);

#endif
