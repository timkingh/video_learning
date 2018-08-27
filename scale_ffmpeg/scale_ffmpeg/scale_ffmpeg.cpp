#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "stdint.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

using namespace std;

int main()
{
    int ret = 0;
    cout << "Start!" << endl;
    string in_path = "F:\\rkvenc_verify\\input_yuv\\Bus_352x288_25.YUV420sp";
    string out_path = "F:\\rkvenc_verify\\input_yuv\\Bus_out.yuv";

    cout << "input: " << in_path << endl
         << "output: " << out_path << endl;
    istream *ifs = new ifstream(in_path.c_str(), ios::binary | ios::in);
    ofstream ofs;
    ofs.open(out_path.c_str(), ios::binary | ios::out);

    uint32_t frames = 10, frame_cnt = 0;
    uint32_t width = 352, height = 288;
    uint32_t frame_size = width * height * 3 / 2;
    char *buf = new char[frame_size];
    uint8_t *in_data[3];
    uint32_t out_width = 352, out_height = 288;

    /* Todo: impacted by src fmt */
    in_data[0] = (uint8_t *)buf;
    in_data[1] = in_data[0] + width * height;
    in_data[2] = in_data[0] + width * height;

    AVFrame *pFrameYUV;
    unsigned char *out_buffer;
    int src_linesize[4] = {0};

    pFrameYUV = av_frame_alloc();
    ret = av_image_fill_linesizes(src_linesize, AV_PIX_FMT_NV12, width);
    if (ret < 0) {
        cout << "fill src linesize error ret " << ret << endl;
    }

    uint32_t out_frame_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, out_width, out_height, 1);
    cout << "buffer size " << out_frame_size << endl;
    out_buffer = (unsigned char *)av_malloc(out_frame_size);
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
                         AV_PIX_FMT_YUV420P, out_width, out_height, 1);

    struct SwsContext *img_convert_ctx;
    img_convert_ctx = sws_getContext(width, height, AV_PIX_FMT_NV12,
                                     out_width, out_height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);

    uint32_t y_size = out_width * out_height;
    while (frame_cnt++ < frames) {
        ifs->read(buf, frame_size);

        sws_scale(img_convert_ctx, (const unsigned char* const*)in_data,
                  src_linesize, 0, height,
                  pFrameYUV->data, pFrameYUV->linesize);
        cout << "frame_cnt " << frame_cnt << endl;
        ofs.write(reinterpret_cast<char*>(pFrameYUV->data[0]), y_size);
        ofs.write(reinterpret_cast<char*>(pFrameYUV->data[1]), y_size / 4);
        ofs.write(reinterpret_cast<char*>(pFrameYUV->data[2]), y_size / 4);
    }

    cout << "End!" << endl;

    if (ifs && ifs != &cin)
        delete ifs;

    ofs.close();

    if (buf)
        delete [] buf;

    if (out_buffer)
        av_free(out_buffer);

    if (pFrameYUV)
        av_frame_free(&pFrameYUV);

    return 0;
}
