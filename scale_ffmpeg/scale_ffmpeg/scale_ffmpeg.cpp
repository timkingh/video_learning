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
    cout << "Start!" << endl;
    string in_path = "F:\\yuv\\yyq.yuv";
    string out_path = "F:\\yuv\\yyq_out.yuv";

    cout << "input: " << in_path << endl
         << "output: " << out_path << endl;
    istream *ifs = new ifstream(in_path.c_str(), ios::binary | ios::in);
    ofstream ofs;
    ofs.open(out_path.c_str(), ios::binary | ios::out);

    uint32_t frames = 10, frame_cnt = 0;
    uint32_t width = 1280, height = 720;
    uint32_t frame_size = width * height * 3 / 2;
    char *buf = new char[frame_size];
    uint8_t *in_data[3];
    uint32_t out_width = 640, out_height = 480;
    in_data[0] = (uint8_t *)buf;
    in_data[1] = in_data[0] + width * height;
    in_data[2] = in_data[1] + width * height / 4;

    cout << "log level " << av_log_get_level() << endl;
    av_log_set_level(56);
    cout << "log level " << av_log_get_level() << endl;

    AVFrame *pFrameYUV;
    unsigned char *out_buffer;
    int linesize[3] = {0};
    linesize[0] = width;
    linesize[1] = linesize[2] = width / 2;
    pFrameYUV = av_frame_alloc();

    uint32_t out_frame_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, out_width, out_height, 1);
    cout << "buffer size " << out_frame_size << endl;
    out_buffer = (unsigned char *)av_malloc(out_frame_size);
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
                         AV_PIX_FMT_YUV420P, out_width, out_height, 1);

    struct SwsContext *img_convert_ctx;
    img_convert_ctx = sws_getContext(width, height, AV_PIX_FMT_YUV420P,
                                     out_width, out_height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);

    uint32_t y_size = out_width * out_height;
    while (frame_cnt++ < frames) {
        ifs->read(buf, frame_size);

        sws_scale(img_convert_ctx, (const unsigned char* const*)in_data,
                  linesize, 0, height,
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
