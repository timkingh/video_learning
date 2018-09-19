#include <iostream>
#include <fstream>
#include <string.h>  /* needed by sscanf */
#include <string>
#include <vector>
#include "getopt.hpp"
#include "common.h"
#include "generate_yuv.h"

using namespace std;

#ifdef _WIN32
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif

static void generate_yuv(GenCtx *ctx)
{
    uint32_t luma_size = ctx->width * ctx->height;
    uint32_t chroma_size = luma_size / 2;
    uint32_t frame_size = luma_size + chroma_size;
    uint32_t left = ctx->left;
    uint32_t top = ctx->top;
    uint32_t right = ctx->right;
    uint32_t bottom = ctx->bottom;
    uint32_t frame_cnt, region_num, region_idx;
    char lines[512];
    char *buf = new char[frame_size];

    ctx->ifs->read(buf, frame_size);

    do {
        ctx->ofs->write(buf, frame_size);

        ctx->ifs->read(buf, frame_size);
        ctx->frame_read++;
    } while (ctx->frame_read < ctx->frames);

    delete [] buf;
}

int main(int argc, char **argv)
{
    GenCtx proc_ctx;
    GenCtx *ctx = &proc_ctx;
    memset(ctx, 0, sizeof(GenCtx));

    /* -c 3903.md --- Motion detected regions of Hisilicon
     * -s   *.sad --- Motion detected regions of RK
     */
    cout << "----------Test-------------" << endl;
    bool help = getarg(false, "-H", "--help", "-?");
    string in_file = getarg("F:\\rkvenc_verify\\input_yuv\\3903_720x576.yuv", "-i", "--input");
    string out_file = getarg("F:\\rkvenc_verify\\input_yuv\\3903_720x576_hi_rk.yuv", "-o", "--output");
    ctx->width = getarg(720, "-w", "--width");
    ctx->height = getarg(576, "-h", "--height");
    ctx->left = getarg(10, "-l", "--left");
    ctx->top = getarg(20, "-t", "--top");
    ctx->right = getarg(50, "-r", "--right");
    ctx->bottom = getarg(80, "-b", "--bottom");
    ctx->frames = getarg(2, "-f", "--frames");
    ctx->motion_rate_thresh = getarg(50, "-m", "--motion_thresh");
    ctx->enable_draw_dot = getarg(1, "-dd", "--draw_dot");
    ctx->draw_blue_dot = getarg(1, "-dbd", "--draw_blue_dot");
    ctx->draw_blue_rect = getarg(0, "-dbr", "--draw_blue_rect");

    if (help) {
        cout << "Usage:" << endl
             << "./yuv_process -i=3903_720x576.yuv -o=3903_720x576_hi_rk.yuv "
             << "-c=3903.md -s=3903_720x576_150.sad -f=2"
             << endl;
        return 0;
    }

    cout << "input: " << in_file << endl
         << "output: " << out_file << endl;
    ctx->ifs = new ifstream(in_file.c_str(), ios::binary | ios::in);

    ofstream ofs;
    ctx->ofs = &ofs;
    ofs.open(out_file.c_str(), ios::binary | ios::out);

    generate_yuv(ctx);

    if (ctx->ifs && ctx->ifs != &cin)
        delete ctx->ifs;
    ofs.close();

    cout << "----------End!-------------" << endl;
    //string str;
    //cin >> str;
    return 0;
}
