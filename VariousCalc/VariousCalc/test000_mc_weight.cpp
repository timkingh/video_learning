#include "common.h"

#define PIXEL_MAX 255
static uint8_t x264_clip_pixel(int x)
{
	return ((x & ~PIXEL_MAX) ? (-x) >> 31 & PIXEL_MAX : x);
}

static int x264_clip3(int v, int i_min, int i_max)
{
	return ((v < i_min) ? i_min : (v > i_max) ? i_max : v);
}

static bool isSame(uint8_t data[], uint8_t len)
{
	bool same = true;
	for (uint8_t i = 0; i < len - 1; i++) {
		same &= (data[i] == data[i + 1]);
		if (!same)
			break;
	}
	return same;
}

void test000_mc_weight(void *ctx)
{
	printf("%s enter\n", __FUNCTION__);
	uint8_t dst[8];
	uint8_t j = 0;
	uint32_t denom, scale;
	int32_t offset = -2;
	uint32_t start_denom = 3, end_denom = 7;

	for (int i = 16; i <= PIXEL_MAX; i++) {
		j = 0;

		for (denom = start_denom; denom <= end_denom; denom++) {
			scale = 9 << (denom - start_denom);
			scale = x264_clip3(scale, 0, 127);
			dst[j++] = x264_clip_pixel(((i * scale + (1 << (denom - 1))) >> denom) + offset);
		}

		if (!isSame(dst, j)) {
			for (uint8_t k = 0; k < j; k++) {
				printf("pixel %d k %d dst %d\n", i, k, dst[k]);
			}
		}
	}
}