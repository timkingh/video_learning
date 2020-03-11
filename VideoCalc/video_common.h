#ifndef __VIDEO_COMMON_H__
#define __VIDEO_COMMON_H__

#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#include <stdint.h>
#include <assert.h>

#include <vector>
#include <fstream>

using namespace std;

#define     FPRINT(fp, ...)  { if (fp) { fprintf(fp, ## __VA_ARGS__);} }
#define     FPCLOSE(fp, ...) { if (fp) { fclose(fp); fp = NULL;} }

#ifdef _WIN32
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif

enum RET {
	RET_NOK = -1,
	RET_OK = 0,
};

typedef struct {
	string input;
	string input_cmp;
	string output;
	string out_yuv;
	uint32_t width;
	uint32_t height;
	uint32_t frames;
	uint32_t mode;
	uint32_t var_ratio_flg;
	uint32_t log_frames;

    /* histogram */
	vector<vector<int>> hist_org; /* Plane 0 1 2 */
	vector<vector<int>> hist_weight;

    /* quant matrix */
    uint32_t rand_cnt;
    uint32_t rand_seq;
    uint32_t seed;
    uint32_t mf_fixed_point_bits;
    uint32_t bias_fixed_point_bits;
    uint32_t real_fixed_bits;
    uint32_t real_bias_fixed_bits;
    uint32_t mf_diff_thresh;
    uint32_t bias;
    uint64_t sum_diff;
    uint64_t sum_diff_bias;
    uint8_t dump_matrix;
    uint8_t default_matrix;
    uint8_t cqm_4iy[16];
    uint8_t cqm_8iy[64];
} CalcCtx;

typedef RET(*func)(CalcCtx *);

static inline int video_clip3( int v, int i_min, int i_max )
{
    return ( (v < i_min) ? i_min : (v > i_max) ? i_max : v );
}

int64_t time_mdate(void);

#endif