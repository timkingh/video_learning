#ifndef __YUV_PROCESS_H__
#define __YUV_PROCESS_H__

#include <stdint.h>
#include "rectangle.h"

typedef struct {
    uint32_t frame_cnt;
    uint32_t mb_size;
    uint32_t mb_width;
    uint32_t mb_height;
    uint32_t mb_x;
    uint32_t mb_y;
} SadInfo;

typedef struct ProcCtx {
    uint32_t width;
    uint32_t height;
    uint32_t mb_size; /* default: 4 */
    uint32_t left;
    uint32_t top;
    uint32_t right;
    uint32_t bottom;
    uint32_t frames;
    uint32_t frame_read;
    uint32_t motion_rate_thresh;
    uint8_t  enable_draw_dot;
    uint8_t  draw_blue_dot;
    uint8_t  draw_blue_rect;
    uint8_t  judge_neighbor;
    uint8_t  judge_intersect;
    std::string coord_file; /* coordinates of motion blocks, from Hisilicon */
    std::string sad_file; /* coordinates of motion blocks, from Rockchip */
    YuvInfo  yuv_info;
    SadInfo  sad_info;
    std::istream *ifs;
    std::ofstream *ofs;
} ProcCtx;

#endif
