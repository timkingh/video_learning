#include <stdio.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include "stdint.h"
#include "getopt.hpp"
#include "ff_tools.h"

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
    int disp_flg;
    int out_scale; /* scale factor for output */
    int madi_thd;
    int madp_thd;

    string in_filename;
    string out_filename;

    ofstream *ofs;
} CliCtx;

static RET tools_init(CliCtx *ctx, ToolsCtx *t)
{
    RET ret = RET_OK;

    t->in_filename = ctx->in_filename.c_str();
    t->out_filename = ctx->out_filename.c_str();
    t->frames = ctx->frames;
    t->width = ctx->width_in;
    t->height = ctx->height_in;
    t->pix_fmt = ctx->input_fmt;

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

    memset((void *)ctx, 0, sizeof(CliCtx));
    memset((void *)tools, 0, sizeof(ToolsCtx));

    cout << "----> FF TOOLS Start ----> " << endl;

    bool help = getarg(false, "-H", "--help", "-?");
    ctx->in_filename = getarg("/home/timkingh/yuv/311_1080p_10000kbps_h265_300frames.yuv", "-i", "--input");
    ctx->out_filename = getarg("/home/timkingh/yuv/out_1080p.yuv", "-o", "--output");
    ctx->width_in = getarg(1920, "-wi", "--width");
    ctx->height_in = getarg(1080, "-hi", "--height");
    ctx->input_fmt = getarg(0, "--input_fmt");
    ctx->width_out = getarg(ctx->width_in, "-wo");
    ctx->height_out = getarg(ctx->height_in, "-ho");
    ctx->frames = getarg(3, "-f", "--frames");
    ctx->disp_flg = getarg(1, "--disp_flg");
    ctx->out_scale = getarg(2, "--out_scale");
    ctx->madi_thd = getarg(1, "--madi_thd");
    ctx->madp_thd = getarg(1, "--madp_thd");

    if (help || argc == 1) {
        cout << "Usage: " << argv[0] << " [options]" << endl
             << "Options:" << endl
             << "  -i, --input <file>    input file name" << endl
             << "  -o, --output <file>   output file name" << endl
             << "  -wi, --width <num>    input width" << endl
             << "  -hi, --height <num>   input height" << endl
             << "  --input_fmt <num>     input format (0 - yuv420p, 1 - yuv420sp)" << endl
             << "  -wo, --width_out <num>   output width" << endl
             << "  -ho, --height_out <num>  output height" << endl
             << "  -f, --frames <num>    number of frames to process" << endl
             << "  --disp_flg <num>      display flag(1 - madi, 2 - madp)" << endl
             << "  --out_scale <num>     output scale factor" << endl
             << "  --madi_thd <num>      madi threshold" << endl
             << "  --madp_thd <num>      madp threshold" << endl
             << "  -H, --help            display this help message" << endl;
        return 0;
    }

    cout << "input: " << ctx->in_filename << endl
         << "output: " << ctx->out_filename << endl
         << "frames: " << ctx->frames << endl;

    ret = tools_init(ctx, tools);
    if (ret == RET_OK) {
        draw_text_filter_ff(tools);
    }

    tools_deinit(ctx, tools);

    cout << "----> FF TOOLS End ----> " << endl;

    return 0;
}
