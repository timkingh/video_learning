#ifndef _COMMON_FF_H_
#define _COMMON_FF_H_

#include <stdio.h>

typedef enum {
	RET_NOK = -1,
	RET_OK = 0,
} RET;

typedef struct {
    const char *in_filename;
    const char *out_filename;
    int width;
    int height;
    int fps;
    int frames;
    FILE *fp_in;
    FILE *fp_out;
} ToolsCtx;

#endif
