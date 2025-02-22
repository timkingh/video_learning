#ifndef _COMMON_FF_H_
#define _COMMON_FF_H_

#include <stdio.h>
#include <stdint.h>

#define FCLOSE(fp) { if (fp) { fclose(fp); fp = NULL; } }

typedef enum {
	RET_NOK = -1,
	RET_OK = 0,
} RET;

typedef enum {
    DISP_MADI = 0,
    DISP_MADP = 1,
    DISP_DSPY = 2,
    DISP_NN_RET = 3
} DispType;

typedef struct {
    DispType disp_flg;
    int out_scale;
    int madi_thd;
    int madp_thd;
} DrawTextParam;

typedef struct {
    const char *in_filename;
    const char *in_nn_results;
    const char *out_filename;
    const char *out_file_dspy;
    int width;
    int height;
    int pix_fmt;
    int fps;
    int frames;
    int aligned_size; /* aligned size of input file */

    DrawTextParam draw_text_param;

    FILE *fp_in;
    FILE *fp_out;
} ToolsCtx;

#ifdef __cplusplus
extern "C" {
#endif
int64_t time_mdate(void);

#ifdef __cplusplus
}    // extern "C"
#endif

#endif
