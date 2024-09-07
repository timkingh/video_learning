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
    uint8_t *in_data[3];

    int disp_flg; /* 0 - no display, 1 - display madi, 2 - display madp */

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
    t->disp_flg = ctx->disp_flg;

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
    ctx->width_out = getarg(ctx->width_in, "-wo");
    ctx->height_out = getarg(ctx->height_in, "-ho");
    ctx->frames = getarg(3, "-f", "--frames");
    ctx->disp_flg = getarg(1, "--disp_flg");

    if (help) {
        cout << "Usage: " << argv[0] << " [options]" << endl
             << "Options:" << endl
             << "  -i, --input <file>    input file name" << endl
             << "  -o, --output <file>   output file name" << endl
             << "  -wi, --width <num>    input width" << endl
             << "  -hi, --height <num>   input height" << endl
             << "  -wo, --width_out <num>   output width" << endl
             << "  -ho, --height_out <num>  output height" << endl
             << "  -f, --frames <num>    number of frames to process" << endl
             << "  --disp_flg <num>      display flag" << endl
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
