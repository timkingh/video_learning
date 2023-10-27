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

#define INVALID_PIXEL      (256)
#define CU16x16_WIDTH      (22)
#define CU4x4_WIDTH        (88)

typedef struct BlkInfo {
    uint32_t y_left;
    uint32_t y_top;
    uint32_t uv_left;
    uint32_t uv_top;
    uint32_t mb_size;
    uint32_t y_pixel;
    uint32_t u_pixel;
    uint32_t v_pixel;
} BlkInfo;

static void fill_block(BufInfo *buf_info, BlkInfo *blk_info)
{
    uint32_t col, row;
    uint32_t x0, y0, mb_size;
    uint32_t width = buf_info->width;
    uint32_t height = buf_info->height;
    char *buf_y = buf_info->buf;
    char *buf_u = buf_info->buf + width * height;
    char *buf_v = buf_u + width * height / 4;
    char *buf;
    x0 = blk_info->y_left;
    y0 = blk_info->y_top;
    mb_size = blk_info->mb_size;

    /* Y */
    if (blk_info->y_pixel != INVALID_PIXEL) {
        buf = buf_y + x0 + y0 * width;
        for (row = 0; row < mb_size; row++) {
            for (col = 0; col < mb_size; col++) {
                buf[row * width + col] = blk_info->y_pixel;
            }
        }
    }

    uint32_t chroma_width = width / 2;
    x0 = blk_info->uv_left;
    y0 = blk_info->uv_top;

    /* U */
    if (blk_info->u_pixel != INVALID_PIXEL) {
        buf = buf_u + x0 + y0 * chroma_width;
        for (row = 0; row < mb_size; row++) {
            for (col = 0; col < mb_size; col++) {
                buf[row * chroma_width + col] = blk_info->u_pixel;
            }
        }
    }

    /* V */
    if (blk_info->v_pixel != INVALID_PIXEL) {
        buf = buf_v + x0 + y0 * chroma_width;
        for (row = 0; row < mb_size; row++) {
            for (col = 0; col < mb_size; col++) {
                buf[row * chroma_width + col] = blk_info->v_pixel;
            }
        }
    }
}

static void fill_blocks(GenCtx *ctx, char *buf, vector<BlkInfo> &blocks)
{
    BlkInfo blk_info;
    BufInfo buf_info;
    buf_info.buf = buf;
    buf_info.width = ctx->width;
    buf_info.height = ctx->height;

    for (uint32_t idx = 0; idx < blocks.size(); idx++) {
        blk_info = blocks.at(idx);
        fill_block(&buf_info, &blk_info);
    }

    blocks.clear();
}

static void gen_blocks_info(GenCtx *ctx, vector<BlkInfo> &blocks)
{
    /* CU4x4 raster scan */
    uint32_t cu_idx[] = {
        CU16x16_WIDTH * 0, CU16x16_WIDTH * 0 + 1, CU16x16_WIDTH * 0 + 2, CU16x16_WIDTH * 0 + 3,
        CU16x16_WIDTH * 1, CU16x16_WIDTH * 1 + 1, CU16x16_WIDTH * 1 + 2, CU16x16_WIDTH * 1 + 3,
        CU16x16_WIDTH * 2, CU16x16_WIDTH * 2 + 1, CU16x16_WIDTH * 2 + 2, CU16x16_WIDTH * 2 + 3,
        CU16x16_WIDTH * 3, CU16x16_WIDTH * 3 + 1, CU16x16_WIDTH * 3 + 2, CU16x16_WIDTH * 3 + 3
    };
    uint32_t pix_idx[] = {0,  255,   0, 255,
                          255,  0, 255,   0,
                          0,  255,   0, 255,
                          255,  0, 255,   0,
    };
    uint32_t mb_width = ctx->width / ctx->mb_size;
    uint32_t mb_height = ctx->height / ctx->mb_size;
    uint32_t idx;
    BlkInfo blk_info;
    blk_info.mb_size = ctx->mb_size;
    blk_info.y_pixel = INVALID_PIXEL;
    blk_info.u_pixel = INVALID_PIXEL;
    blk_info.v_pixel = INVALID_PIXEL;

    for (idx = 0; idx < sizeof(cu_idx) / sizeof(uint32_t); idx++) {
        blk_info.y_left = (cu_idx[idx] % mb_width) * ctx->mb_size;
        blk_info.y_top = (cu_idx[idx] / mb_width) * ctx->mb_size;
        blk_info.y_pixel = pix_idx[idx];
        blocks.push_back(blk_info);
    }
}

static void handle_one_frame(GenCtx *ctx, char *buf)
{
    switch(ctx->frame_read) {
        case 0 :{
            vector<BlkInfo> blocks;
            gen_blocks_info(ctx, blocks);
            fill_blocks(ctx, buf, blocks);
            break;
        } case 1 : case 3 :{
            memset(buf, 0, ctx->frame_size);
            break;
        } case 2 : case 4 :{
            memset(buf, 255, ctx->frame_size);
            break;
        } default :{
        }
    }

    if (ctx->frame_read > 4 && ctx->frame_read < 13) {
        static uint8_t pixels[8][3] = {
                {0, 0, 0},   {255, 0, 0},   {0, 255, 0},   {255, 255, 0},
                {0, 0, 255}, {255, 0, 255}, {0, 255, 255}, {255, 255, 255},
        };
        uint8_t y_pixel, u_pixel, v_pixel;
        uint8_t idx;

        idx = ctx->frame_read - 5;
        y_pixel = pixels[idx][0];
        u_pixel = pixels[idx][1];
        v_pixel = pixels[idx][2];

        uint32_t luma_len = ctx->width * ctx->height;
        uint32_t chroma_len = luma_len / 4;
        char *buf_y = buf;
        char *buf_u = buf + luma_len;
        char *buf_v = buf_u + chroma_len;

        memset(buf_y, y_pixel, luma_len);
        memset(buf_u, u_pixel, chroma_len);
        memset(buf_v, v_pixel, chroma_len);
    }
}

static void generate_yuv(GenCtx *ctx)
{
    uint32_t luma_size = ctx->width * ctx->height;
    uint32_t chroma_size = luma_size / 2;
    uint32_t frame_size = luma_size + chroma_size;
    char *buf = new char[frame_size];
    ctx->frame_size = frame_size;

    do {
        ctx->ifs->read(buf, frame_size);

        handle_one_frame(ctx, buf);

        ctx->ofs->write(buf, frame_size);

        ctx->frame_read++;
    } while (ctx->frame_read < ctx->frames);

    delete [] buf;
}

int main(int argc, char **argv)
{
    GenCtx proc_ctx;
    GenCtx *ctx = &proc_ctx;
    memset(ctx, 0, sizeof(GenCtx));

    cout << "----------Test-------------" << endl;
    bool help = getarg(false, "-H", "--help", "-?");
    string in_file = getarg("/home/timkingh/yuv/street_720p_250frames.yuv", "-i", "--input");
    string out_file = getarg("/home/timkingh/yuv/street_720p_out.yuv", "-o", "--output");
    ctx->width = getarg(1280, "-w", "--width");
    ctx->height = getarg(720, "-h", "--height");
    ctx->frames = getarg(1, "-f", "--frames");
    ctx->mb_size = 16;

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
