#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "stdint.h"
#include "getopt.hpp"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

using namespace std;

/* command line context */
typedef struct CliCtx {
    uint32_t frames;
    uint32_t width_in;
    uint32_t height_in;
    uint32_t width_out;
    uint32_t height_out;
    uint8_t *in_data[3];

    ofstream *ofs;

    AVFrame *dst_frame;

    enum AVPixelFormat src_fmt;
    enum AVPixelFormat dst_fmt;
} CliCtx;

static void write_dst_frame_to_file(CliCtx *ctx)
{
    AVFrame *dst = ctx->dst_frame;
    uint32_t width = ctx->width_in;
    uint32_t height = ctx->height_in;

    if (ctx->dst_fmt == AV_PIX_FMT_YUV420P) {
        uint32_t y_size = ctx->width_in * ctx->height_in;
        ctx->ofs->write(reinterpret_cast<char*>(dst->data[0]), y_size);
        ctx->ofs->write(reinterpret_cast<char*>(dst->data[1]), y_size / 4);
        ctx->ofs->write(reinterpret_cast<char*>(dst->data[2]), y_size / 4);
    } else if (ctx->dst_fmt == AV_PIX_FMT_RGB24) {
        ctx->ofs->write(reinterpret_cast<char*>(dst->data[0]),
                        dst->linesize[0] * height);
    }
}


int main(int argc, char **argv)
{
    int ret = 0;
    CliCtx cli_ctx;
    CliCtx *ctx = &cli_ctx;
    memset(ctx, 0, sizeof(CliCtx));
    cout << "Start!" << endl;

    bool help = getarg(false, "-H", "--help", "-?");
    string in_path = getarg("F:\\yuv\\test_cif_420sp.yuv", "-i", "--input");
    string out_path = getarg("F:\\yuv\\test_cif_420p.yuv", "-o", "--output");
    ctx->width_in = getarg(352, "-w", "--width");
    ctx->height_in = getarg(288, "-h", "--height");
    ctx->frames = getarg(10, "-f", "--frames");
    ctx->src_fmt = static_cast<enum AVPixelFormat>(getarg(25, "-src_fmt"));
	ctx->dst_fmt = static_cast<enum AVPixelFormat>(getarg(2, "-dst_fmt"));

    cout << "input: " << in_path << endl
         << "output: " << out_path << endl
         << "frames: " << ctx->frames << endl;
    istream *ifs = new ifstream(in_path.c_str(), ios::binary | ios::in);
    ofstream ofs;
    ctx->ofs = &ofs;
    ofs.open(out_path.c_str(), ios::binary | ios::out);

    uint32_t frame_cnt = 0;
    uint32_t frame_size = ctx->width_in * ctx->height_in * 3 / 2;
    char *buf = new char[frame_size];
    uint8_t *in_data[3];
    uint32_t out_width = ctx->width_in, out_height = ctx->height_in;

    /* Todo: impacted by src fmt */
    in_data[0] = (uint8_t *)buf;
    in_data[1] = in_data[0] + ctx->width_in * ctx->height_in;
    in_data[2] = in_data[0] + ctx->width_in * ctx->height_in;

    unsigned char *out_buffer;
    int src_linesize[4] = {0};

    ctx->dst_frame = av_frame_alloc();
    ret = av_image_fill_linesizes(src_linesize, ctx->src_fmt, ctx->width_in);
    if (ret < 0) {
        cout << "fill src linesize error ret " << ret << endl;
    }

    uint32_t out_frame_size = av_image_get_buffer_size(ctx->dst_fmt, out_width, out_height, 1);
    cout << "buffer size " << out_frame_size << endl;
    out_buffer = (unsigned char *)av_malloc(out_frame_size);
    av_image_fill_arrays(ctx->dst_frame->data, ctx->dst_frame->linesize, out_buffer,
                         ctx->dst_fmt, out_width, out_height, 1);

    struct SwsContext *img_convert_ctx;
    img_convert_ctx = sws_getContext(ctx->width_in, ctx->height_in, ctx->src_fmt,
                                     out_width, out_height, ctx->dst_fmt,
                                     SWS_BICUBIC, NULL, NULL, NULL);

    while (frame_cnt++ < ctx->frames) {
        ifs->read(buf, frame_size);

        sws_scale(img_convert_ctx, (const unsigned char * const*)in_data,
                  src_linesize, 0, ctx->height_in,
                  ctx->dst_frame->data, ctx->dst_frame->linesize);
        cout << "frame_cnt " << frame_cnt << endl;
        write_dst_frame_to_file(ctx);
    }

    cout << "End!" << endl;

    if (ifs && ifs != &cin)
        delete ifs;

    ctx->ofs->close();

    if (buf)
        delete [] buf;

    if (out_buffer)
        av_free(out_buffer);

    if (ctx->dst_frame)
        av_frame_free(&ctx->dst_frame);

    return 0;
}
