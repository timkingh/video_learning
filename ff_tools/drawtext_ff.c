/*
 * Copyright (c) 2010 Nicolas George
 * Copyright (c) 2011 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * API example for decoding and filtering
 * @example filtering_video.c
 */

#define _XOPEN_SOURCE 600 /* for usleep */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/opt.h"
#ifdef __cplusplus
}
#endif

#include "common_ff.h"

typedef struct {
    AVFormatContext *fmt_ctx;
    AVCodecContext *dec_ctx;
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;

    ToolsCtx *tools_ctx;
    const char *in_filename;
    const char *out_filename;
    int is_yuv_input;
    int width;
    int height;
    int frames_to_filter;
    int frm_cnt;

    FILE *fp_in;
    FILE *fp_out;
} DrawTextCtx;

const char *filter_descr = "scale=3840:2160,drawtext=fontfile=/usr/share/fonts/truetype/freefont/FreeSans.ttf:fontsize=10:text='1234567890':fontcolor=red:x=10:y=10:enable='between(t,0,10)';";
/* other way:
   scale=78:24 [scl]; [scl] transpose=cclock // assumes "[in]" and "[out]" to be input output pads respectively
 */

AVFilterContext *buffersink_ctx;
AVFilterContext *buffersrc_ctx;
AVFilterGraph *filter_graph;
static int video_stream_index = -1;
static int64_t last_pts = AV_NOPTS_VALUE;

static RET draw_txt_filter_init(ToolsCtx *ctx, DrawTextCtx *dtc)
{
    memset(dtc, 0, sizeof(DrawTextCtx));
    dtc->tools_ctx = ctx;
    dtc->frames_to_filter = ctx->frames;
    dtc->in_filename = ctx->in_filename;
    dtc->out_filename = ctx->out_filename;
    dtc->width = ctx->width;
    dtc->height = ctx->height;
    dtc->is_yuv_input = 1;

    dtc->fp_out = fopen(dtc->out_filename, "wb");
    if (!dtc->fp_out) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open output file %s\n", dtc->out_filename);
        return RET_NOK;
    }

    return RET_OK;
}

static RET draw_txt_filter_deinit(DrawTextCtx *dtc)
{
    avcodec_free_context(&dtc->dec_ctx);
    avformat_close_input(&dtc->fmt_ctx);

    if (dtc->fp_out) {
        fclose(dtc->fp_out);
        dtc->fp_out = NULL;
    }

    return RET_OK;
}

static int open_input_file(DrawTextCtx *dtc)
{
    const AVCodec *dec;
    int ret;

    if ((ret = avformat_open_input(&dtc->fmt_ctx, dtc->in_filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(dtc->fmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    /* select the video stream */
    ret = av_find_best_stream(dtc->fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find a video stream in the input file\n");
        return ret;
    }
    video_stream_index = ret;

    /* create decoding context */
    dtc->dec_ctx = avcodec_alloc_context3(dec);
    if (!dtc->dec_ctx)
        return AVERROR(ENOMEM);
    avcodec_parameters_to_context(dtc->dec_ctx, dtc->fmt_ctx->streams[video_stream_index]->codecpar);

    /* init the video decoder */
    if ((ret = avcodec_open2(dtc->dec_ctx, dec, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
        return ret;
    }

    return 0;
}

static int open_input_yuv(DrawTextCtx *dtc)
{
    RET ret = RET_OK;
    AVFormatContext *format_ctx = NULL;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *codec = NULL;
    AVFrame *frame = NULL;
    AVPacket packet;
    const AVInputFormat *fmt = av_find_input_format("rawvideo");
    // format_ctx = avformat_alloc_context();
    AVDictionary *options = NULL;
    av_dict_set(&options, "video_size", "1920x1080", 0);
    av_dict_set(&options, "pixel_format", "yuv420p", 0);

    if ((ret = avformat_open_input(&format_ctx, dtc->in_filename, fmt, &options)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }
    video_stream_index = 0;
    // format_ctx->iformat = av_guess_format("rawvideo", NULL, NULL);

    codec = avcodec_find_decoder_by_name("rawvideo");
    if (!codec) {
        fprintf(stderr, "Codec 'rawvideo' not found\n");
        return RET_NOK;
    }

    // 分配解码器上下文
    dec_ctx = avcodec_alloc_context3(codec);
    if (!dec_ctx) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return RET_NOK;
    }

    // 设置解码器上下文参数
    dec_ctx->codec_id = codec->id; //AV_CODEC_ID_RAWVIDEO
    dec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    dec_ctx->bit_rate = 400000; // 设置码率，对于rawvideo编解码器，这通常没有实际作用
    dec_ctx->width = 1920;
    dec_ctx->height = 1080;
    dec_ctx->pix_fmt = AV_PIX_FMT_YUV420P; // 设置像素格式为YUV420P
    dec_ctx->time_base = (AVRational){1, 25}; // 设置时间基，对于rawvideo编解码器，这通常没有实际作用
    dec_ctx->framerate = (AVRational){25, 1}; // 设置帧率，对于rawvideo编解码器，这通常没有实际作用
    dec_ctx->sample_aspect_ratio = (AVRational){1, 1}; // 设置采样率，对于rawvideo编解码器，这通常没有实际作用

    if (avcodec_open2(dec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return RET_NOK;
    }

    // // 分配并初始化一个AVFrame
    // frame = av_frame_alloc();
    // if (!frame) {
    //     fprintf(stderr, "Could not allocate video frame\n");
    //     return RET_NOK;;
    // }

    // 打开输入文件
    if (avio_open(&format_ctx->pb, dtc->in_filename, AVIO_FLAG_READ) < 0) {
        fprintf(stderr, "Could not open input file\n");
        return RET_NOK;
    }

    // // 读取YUV数据并解码
    // while (av_read_frame(format_ctx, &packet) >= 0) {
    //     // 解码过程...
    //     // 注意：这里你需要手动处理YUV数据的读取和填充到AVPacket结构中

    //     // 释放AVPacket
    //     av_packet_unref(&packet);
    // }

    // // 清理和关闭
    // av_frame_free(&frame);
    // avcodec_close(codec_ctx);
    // avformat_close_input(&format_ctx);

    dtc->fmt_ctx = format_ctx;
    dtc->dec_ctx = dec_ctx;

    // // 清理
    // avcodec_free_context(&codec_ctx);

    av_dict_free(&options);
    return ret;
}

static int init_filters(DrawTextCtx *dtc, const char *filters_descr)
{
    char args[512];
    int ret = 0;
    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVRational time_base = dtc->is_yuv_input ? (AVRational){ 1, 25 } :
                           dtc->fmt_ctx->streams[video_stream_index]->time_base;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };

    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            dtc->dec_ctx->width, dtc->dec_ctx->height, dtc->dec_ctx->pix_fmt,
            time_base.num, time_base.den,
            dtc->dec_ctx->sample_aspect_ratio.num, dtc->dec_ctx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        goto end;
    }

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
        goto end;
    }

    /*
     * Set the endpoints for the filter graph. The filter_graph will
     * be linked to the graph described by filters_descr.
     */

    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                    &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

static void dump_frame(DrawTextCtx *dtc, const AVFrame *frame, AVRational time_base)
{
    int x, y;
    uint8_t *p0, *p;
    int64_t delay;

    if (frame->pts != AV_NOPTS_VALUE) {
        if (last_pts != AV_NOPTS_VALUE) {
            /* sleep roughly the right amount of time;
             * usleep is in microseconds, just like AV_TIME_BASE. */
            delay = av_rescale_q(frame->pts - last_pts,
                                 time_base, AV_TIME_BASE_Q);
            if (delay > 0 && delay < 1000000)
                usleep(delay);
        }
        last_pts = frame->pts;
    }

    if (dtc->fp_out) {
        fwrite(frame->data[0], 1, frame->linesize[0] * frame->height, dtc->fp_out);
        fwrite(frame->data[1], 1, frame->linesize[1] * frame->height / 2, dtc->fp_out);
        fwrite(frame->data[2], 1, frame->linesize[2] * frame->height / 2, dtc->fp_out);
    }
}

int draw_text_filter_ff(ToolsCtx *ctx)
{
    DrawTextCtx draw_text_ctx;
    DrawTextCtx *dtc = &draw_text_ctx;
    AVPacket *packet;
    AVFrame *frame;
    AVFrame *filt_frame;
    int ret;

    ret = draw_txt_filter_init(ctx, dtc);
    if (ret != RET_OK) {
        av_log(NULL, AV_LOG_ERROR, "drawtxt filter init failed\n");
        goto end;
    }

    frame = av_frame_alloc();
    filt_frame = av_frame_alloc();
    packet = av_packet_alloc();
    if (!frame || !filt_frame || !packet) {
        fprintf(stderr, "Could not allocate frame or packet\n");
        exit(1);
    }

    if (dtc->is_yuv_input) {
        ret = open_input_yuv(dtc);
        if (ret != RET_OK) {
            av_log(NULL, AV_LOG_ERROR, "open input yuv %s failed\n", dtc->in_filename);
            goto end;
        }
    } else {
        if ((ret = open_input_file(dtc)) < 0)
            goto end;
    }

    if ((ret = init_filters(dtc, filter_descr)) < 0)
        goto end;

    /* read all packets */
    do {
        if ((ret = av_read_frame(dtc->fmt_ctx, packet)) < 0)
            break;

        if (packet->stream_index == video_stream_index) {
            ret = avcodec_send_packet(dtc->dec_ctx, packet);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
                break;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(dtc->dec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
                    goto end;
                }

                frame->pts = frame->best_effort_timestamp;

                // 在将帧发送到滤镜图之前，更新滤镜参数
                char args[512];
                if (dtc->frm_cnt < 1)
                    snprintf(args, sizeof(args), "10");
                else
                    snprintf(args, sizeof(args), "20");


                avfilter_graph_send_command(filter_graph, "drawtext", "x", args, NULL, 0, 0);

                /* push the decoded frame into the filtergraph */
                if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
                    break;
                }

                /* pull filtered frames from the filtergraph */
                while (1) {
                    ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    if (ret < 0)
                        goto end;
                    dump_frame(dtc, filt_frame, buffersink_ctx->inputs[0]->time_base);
                    av_frame_unref(filt_frame);
                }
                av_frame_unref(frame);
            }
        }
        av_packet_unref(packet);
    } while (dtc->frm_cnt++ < dtc->frames_to_filter);

    av_log(NULL, AV_LOG_INFO, "Filtered %d frames\n", dtc->frm_cnt);

end:
    avfilter_graph_free(&filter_graph);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    av_packet_free(&packet);

    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        exit(1);
    }

    draw_txt_filter_deinit(dtc);

    return ret;
}