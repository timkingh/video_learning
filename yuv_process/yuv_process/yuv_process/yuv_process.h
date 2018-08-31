#ifndef __YUV_PROCESS_H__
#define __YUV_PROCESS_H__
#include <stdint.h>

typedef struct {
    uint32_t frame_cnt;
    uint32_t mb_size;
    uint32_t mb_width;
    uint32_t mb_height;
    uint32_t mb_x;
    uint32_t mb_y;
} SadInfo;

struct ProcCtx{
    uint32_t frames;
    YuvInfo yuv_info;
};















#endif
