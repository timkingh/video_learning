#include "draw_num_on_yuv.h"

typedef struct {
    uint8_t *src_buf;
    uint8_t *dst_buf;
    FILE *fp_in;
    FILE *fp_out;
} DrawNumCtx;

static RET draw_number_on_yuv_init(CalcCtx *calc_ctx, DrawNumCtx *draw_ctx)
{

}

RET draw_number_on_yuv(CalcCtx *ctx)
{
	DrawNumCtx draw_ctx;
	memset(&draw_ctx, 0, sizeof(DrawNumCtx));

	draw_number_on_yuv_init(ctx, &draw_ctx);
}