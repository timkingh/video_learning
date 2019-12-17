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

using namespace std;

#define     FPRINT(fp, ...)  { if (fp) { fprintf(fp, ## __VA_ARGS__);} }
#define     FPCLOSE(fp, ...) { if (fp) { fclose(fp); fp = NULL;} }

enum RET {
	RET_NOK = -1,
	RET_OK = 0,
};

typedef struct {
	string input;
	string input_cmp;
	string output;
	uint32_t width;
	uint32_t height;
	uint32_t frames;
	uint32_t mode;
	uint32_t var_ratio_flg;
} CalcCtx;

typedef RET(*func)(CalcCtx *);


int64_t time_mdate(void);

#endif