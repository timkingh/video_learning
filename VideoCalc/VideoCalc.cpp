#define _CRT_SECURE_NO_WARNINGS
#include "video_common.h"
#include "getopt.hpp"
#include "calc_var.h"
#include "calc_psnr.h"
#include "calc_histogram.h"
#include "weight_yuv.h"
#include "quant_matrix.h"
#include "parse_hview_av1_stream.h"
#include "transform_yuv_format.h"

using namespace std;

extern RET cdef_find_dir(CalcCtx *ctx);

enum MODE {
    CALC_PSNR = 0,
    CALC_VAR = 1,
    CALC_HIST = 2, /* Histogram */
    WEIGHT_YUV = 3,
    QUANT_MATRIX = 4,
};

static void show_help()
{
    cout << "Usage: calculate PSNR" << endl
         << "VidoeCalc -i=modify.yuv -o=origin.yuv -m=0 "
         << "-w=1920 -h=1080 --frames=300 -p=psnr.txt --log_frames=10"
         << endl;

    cout << "Usage: calculate Variance" << endl
         << "VideoCalc -i=input.yuv -m=1 --var_ratio_flg=1 "
         << "-w=1920 -h=1080 --frames=300 -p=variance.txt"
         << endl;

    cout << "Usage: calculate histogram" << endl
         << "VidoeCalc -i=modify.yuv -m=2 "
         << "-w=1920 -h=1080 --frames=300 -p=hist.txt"
         << endl;

    cout << "Usage: calculate weighted yuv" << endl
         << "VideoCalc -i=input.yuv -o=weighted.yuv -m=3 "
         << "-w=1920 -h=1080 --frames=300 -p=param.txt --combo_yuv=combo.yuv "
         << endl;

    cout << "Usage: calculate quant matrix" << endl
         << "VideoCalc -m=4 -p=quant.txt --rand_cnt=10 --log_frames=3 "
         << "--mf_diff_thresh=2 --mf_fixed_point_bits=23 --dump_matrix=1 "
         << "--default_matrix=0"
         << endl;
}

static RET buf_init(CalcCtx *ctx)
{
    int buf_len = ctx->width * ctx->height * 2;

    ctx->frm_buf0 = (uint32_t *)calloc(buf_len, sizeof(uint32_t));
    ctx->frm_buf1 = (uint32_t *)calloc(buf_len, sizeof(uint32_t));
    ctx->frm_buf2 = (uint32_t *)calloc(buf_len, sizeof(uint32_t));
    if (ctx->frm_buf0 == NULL || ctx->frm_buf1 == NULL ||
        ctx->frm_buf2 == NULL) {
        printf("malloc %d failed\n", buf_len);
        return RET_NOK;
    }

    for (int k = 0; k < FILE_NUM; k++) {
        ctx->frm_buf[k] = (uint32_t *)calloc(buf_len, sizeof(uint32_t));
        if (ctx->frm_buf[k] == NULL) {
            printf("idx %d malloc %d failed\n", k, buf_len);
            return RET_NOK;
        }
    }

    return RET_OK;
}

static void buf_deinit(CalcCtx *ctx)
{
    FREE(ctx->frm_buf0);
    FREE(ctx->frm_buf1);
    FREE(ctx->frm_buf2);

    for (int k = 0; k < FILE_NUM; k++) {
        FREE(ctx->frm_buf[k]);
    }
}

static RET open_file(CalcCtx *ctx)
{
    for (int k = 0; k < FILE_NUM; k++) {
        if (ctx->out_file[k].c_str()) {
            ctx->fp_out[k] = fopen(ctx->out_file[k].c_str(), "wb");
            if (ctx->fp_out[k] == NULL) {
                printf("fopen output files failed\n");
                return RET_NOK;
            }
        }
    }

    return RET_OK;
}

static void close_file(CalcCtx *ctx)
{
    for (int k = 0; k < FILE_NUM; k++) {
        FPCLOSE(ctx->fp_out[k]);
    }
}

int main(int argc, char **argv)
{
    CalcCtx calc_ctx;
    CalcCtx *ctx = &calc_ctx;
    memset(ctx, 0, sizeof(CalcCtx));
    bool help = getarg(false, "-H", "--help", "-?");
    ctx->input = getarg("modify.yuv", "-i", "--input");
    ctx->input_cmp = getarg("origin.yuv", "-o", "--output");
    ctx->output = getarg("psnr.txt", "-p", "--psnr");
    ctx->out_yuv = getarg("combo.yuv", "-q", "--combo_yuv");

    ctx->width = getarg(1920, "-w", "--width");
    ctx->height = getarg(1080, "-h", "--height");
    ctx->frames = getarg(300, "-f", "--frames");
    ctx->framerate = getarg(30, "--fps");
    ctx->mode = getarg(0, "-m", "--mode");
    ctx->var_ratio_flg = getarg(0, "--var_ratio_flg");
    ctx->log_frames = getarg(1, "--log_frames");

    if (ctx->mode == 1) {
        ctx->out_file[0] = getarg("/mnt/shared/ave.txt", "--out_file0");
        ctx->out_file[1] = getarg("/mnt/shared/var.txt", "--out_file1");
        ctx->out_file[2] = getarg("/mnt/shared/madi.txt", "--out_file2");
        ctx->out_file[3] = getarg("/mnt/shared/ave_uv.txt", "--out_file3");
        ctx->out_file[4] = getarg("/mnt/shared/var_uv.txt", "--out_file4");
        ctx->out_file[5] = getarg("/mnt/shared/madi_uv.txt", "--out_file5");
        ctx->out_file[6] = getarg("/mnt/shared/tmp.txt", "--out_file6");
    }

    /* quant matrix */
    ctx->rand_cnt = getarg(10, "--rand_cnt");
    ctx->dump_matrix = getarg(0, "--dump_matrix");
    ctx->mf_fixed_point_bits = getarg(22, "--mf_fixed_point_bits");
    ctx->bias_fixed_point_bits = getarg(24, "--bias_fixed_point_bits");
    ctx->mf_diff_thresh = getarg(1, "--mf_diff_thresh");
    ctx->default_matrix = getarg(1, "--default_matrix");
    ctx->bias = getarg(1, "--bias");

    if (help || argc < 2) {
        show_help();
        return 0;
    }

    /* Functions' order must match with enum MODE */
    func demo[] = {
        calc_psnr,
        calc_var,
        calc_histogram,
        weight_yuv,
        calc_quant_matrix,
        cdef_find_dir,
        parse_hview_av1_stream,
        transform_yuv_format
    };

    open_file(ctx);
    buf_init(ctx);

    demo[ctx->mode](ctx);

    buf_deinit(ctx);
    close_file(ctx);

    return 0;
}
