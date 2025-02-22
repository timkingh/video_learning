#include <stdio.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include "stdint.h"
#include "getopt.hpp"
#include "ff_tools.h"
#include "common_ff.h"

using namespace std;

/* command line context */
typedef struct CliCtx {
    uint32_t frames;
    uint32_t width_in;
    uint32_t height_in;
    uint32_t width_out;
    uint32_t height_out;
    int input_fmt; /* 0 - yuv420p, 1 - yuv420sp */
    uint8_t *in_data[3];

    /* 0 - no display, 1 - display madi,
     * 2 - display madp, 3 - display dsp of Y */
    DispType disp_flg;
    int out_scale; /* scale factor for output */
    int madi_thd;
    int madp_thd;

    int aligned_size;

    string in_filename;
    string in_nn_results;
    string out_filename;
    string out_filename_dspy;

    ofstream *ofs;
} CliCtx;

static RET tools_init(CliCtx *ctx, ToolsCtx *t)
{
    RET ret = RET_OK;

    t->in_filename = ctx->in_filename.c_str();
    t->in_nn_results = ctx->in_nn_results.c_str();
    t->out_filename = ctx->out_filename.c_str();
    t->out_file_dspy = ctx->out_filename_dspy.c_str();
    t->frames = ctx->frames;
    t->width = ctx->width_in;
    t->height = ctx->height_in;
    t->pix_fmt = ctx->input_fmt;
    t->aligned_size = ctx->aligned_size;

    t->draw_text_param.disp_flg = ctx->disp_flg;
    t->draw_text_param.out_scale = ctx->out_scale;
    t->draw_text_param.madi_thd = ctx->madi_thd;
    t->draw_text_param.madp_thd = ctx->madp_thd;

    return ret;
}

static RET tools_deinit(CliCtx *ctx, ToolsCtx *t)
{
    RET ret = RET_OK;

    return ret;
}

int main(int argc, char **argv)
{
    RET ret = RET_OK;
    CliCtx cli_ctx;
    ToolsCtx tools_ctx;
    CliCtx *ctx = &cli_ctx;
    ToolsCtx *tools = &tools_ctx;
    int64_t start_time = time_mdate();
    int64_t end_time = 0;

    memset((void *)ctx, 0, sizeof(CliCtx));
    memset((void *)tools, 0, sizeof(ToolsCtx));

    cout << "----> FF TOOLS Start ----> " << endl;

    bool help = getarg(false, "-H", "--help", "-?");
    ctx->in_filename = getarg("/home/timkingh/yuv/311_1080p_10000kbps_h265_300frames.yuv", "-i", "--input");
    ctx->in_nn_results = getarg("yolov5_seg_debug_0.txt", "-nn", "--nn_results");
    ctx->out_filename = getarg("/home/timkingh/yuv/out_1080p.yuv", "-o", "--output"); /* support yuv420p only */
    ctx->width_in = getarg(1920, "-wi", "--width");
    ctx->height_in = getarg(1080, "-hi", "--height");
    ctx->input_fmt = getarg(0, "--input_fmt");
    ctx->width_out = getarg(ctx->width_in, "-wo");
    ctx->height_out = getarg(ctx->height_in, "-ho");
    ctx->frames = getarg(3, "-f", "--frames");
    ctx->disp_flg = (DispType)getarg(1, "--disp_flg");
    ctx->out_scale = getarg(2, "--out_scale");
    ctx->madi_thd = getarg(1, "--madi_thd");
    ctx->madp_thd = getarg(1, "--madp_thd");
    ctx->out_filename_dspy = getarg(ctx->in_filename + "_dspy.txt", "--out_filename_dspy");
    ctx->aligned_size = getarg(32, "--aligned_size"); /* 32 for HEVC, 16 for AVC */

    if (help || argc == 1) {
        cout << "Usage: " << argv[0] << " [options]" << endl
             << "Options:" << endl
             << "  -i, --input=<file>    input file name" << endl
             << "  -nn, --nn_results=<file>   nn results file name" << endl
             << "  -o, --output=<file>   output file name" << endl
             << "  -wi, --width=<num>    input width" << endl
             << "  -hi, --height=<num>   input height" << endl
             << "  --input_fmt=<num>     input format (0 - yuv420p, 1 - yuv420sp)" << endl
             << "  -wo, --width_out=<num>   output width" << endl
             << "  -ho, --height_out=<num>  output height" << endl
             << "  -f, --frames=<num>    number of frames to process" << endl
             << "  --disp_flg=<num>      display flag(0 - madi, 1 - madp, 2 - dsp of Y, 3 - NN results)" << endl
             << "  --out_scale=<num>     output scale factor" << endl
             << "  --madi_thd=<num>      madi threshold" << endl
             << "  --madp_thd=<num>      madp threshold" << endl
             << "  -H, --help            display this help message" << endl;
        return 0;
    }

    cout << "input: " << ctx->in_filename << endl
         << "output: " << ctx->out_filename << endl
         << "total frames: " << ctx->frames << endl
         << (ctx->disp_flg == DISP_MADI ? "display MADI" :
            (ctx->disp_flg == DISP_MADP ? "display MADP" :
            (ctx->disp_flg == DISP_DSPY ? "display DSP of Y" : "display NN results"))) << endl;

    ret = tools_init(ctx, tools);
    if (ret == RET_OK) {
        draw_text_filter_ff(tools);
    }

    tools_deinit(ctx, tools);

    end_time = time_mdate();
    printf("calc %d frame(s) elapsed %.2fs\n", ctx->frames, (float)(end_time - start_time) / 1000000);

    cout << "----> FF TOOLS End ----> " << endl;

    return 0;
}
