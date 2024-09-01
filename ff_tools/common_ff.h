#ifndef _COMMON_FF_H_
#define _COMMON_FF_H_

#include <stdio.h>

enum RET {
	RET_NOK = -1,
	RET_OK = 0,
};

typedef struct {
    const char *in_filename;
    const char *out_filename;
    int width;
    int height;
    int fps;
    FILE *fp_in;
    FILE *fp_out;
} ToolsCtx;

#endif
