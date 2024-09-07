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

#define MAX_FILTER_DESC_LEN  (4 * 1024 * 1024)
#define MAX_STR_LEN          (1024)

typedef struct {
    int x, y;
    int blk_size_madi;
    int blk_size_madp;
    int node_id;
    int madi;
    int madp;
} Node;

typedef struct {
    AVFormatContext *fmt_ctx;
    AVCodecContext *dec_ctx;
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
    int video_stream_index;

    ToolsCtx *tools_ctx;
    const char *in_filename;
    const char *out_filename;
    Node *node_list;
    int node_num;
    int node_max_num;
    char *filter_descr;
    unsigned char *cur_frm_data;
    unsigned char *ref_frm_data;
    unsigned char *cur_frm;
    unsigned char *ref_frm;
    int linesize;
    int blk_size_madi;
    int blk_size_madp;
    int is_yuv_input;
    int width;
    int height;
    int frames_to_filter;
    int frm_cnt;

    FILE *fp_in;
    FILE *fp_out;
} DrawTextCtx;

static RET draw_txt_filter_init(ToolsCtx *ctx, DrawTextCtx *dtc)
{
    int width_align, height_align;

    memset(dtc, 0, sizeof(DrawTextCtx));
    dtc->tools_ctx = ctx;
    dtc->frames_to_filter = ctx->frames;
    dtc->in_filename = ctx->in_filename;
    dtc->out_filename = ctx->out_filename;
    dtc->width = ctx->width;
    dtc->height = ctx->height;
    dtc->is_yuv_input = 1;
    dtc->video_stream_index = -1;
    dtc->linesize = FFALIGN(dtc->width, 32);
    dtc->blk_size_madi = 16;
    dtc->blk_size_madp = 32;
    width_align = FFALIGN(dtc->width, 32);
    height_align = FFALIGN(dtc->height, 32);

    dtc->fp_out = fopen(dtc->out_filename, "wb");
    if (!dtc->fp_out) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open output file %s\n", dtc->out_filename);
        return RET_NOK;
    }

    dtc->filter_descr = (char *)calloc(1, MAX_FILTER_DESC_LEN);
    if (!dtc->filter_descr) {
        av_log(NULL, AV_LOG_ERROR, "Cannot allocate filter description\n");
        return RET_NOK;
    }

    dtc->cur_frm_data = (unsigned char *)calloc(1, width_align * height_align * 3 / 2);
    if (!dtc->cur_frm_data) {
        av_log(NULL, AV_LOG_ERROR, "Cannot allocate current frame data\n");
        return RET_NOK;
    }

    dtc->ref_frm_data = (unsigned char *)calloc(1, width_align * height_align * 3 / 2);
    if (!dtc->ref_frm_data) {
        av_log(NULL, AV_LOG_ERROR, "Cannot allocate reference frame data\n");
        return RET_NOK;
    }

    dtc->cur_frm = dtc->cur_frm_data;
    dtc->ref_frm = dtc->ref_frm_data;

    dtc->node_max_num = width_align * height_align / (8 * 8);
    dtc->node_list = (Node *)calloc(dtc->node_max_num, sizeof(Node));
    if (!dtc->node_list) {
        av_log(NULL, AV_LOG_ERROR, "Cannot allocate node list\n");
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

    if (dtc->filter_descr) {
        free(dtc->filter_descr);
        dtc->filter_descr = NULL;
    }

    if (dtc->cur_frm_data) {
        free(dtc->cur_frm_data);
        dtc->cur_frm_data = NULL;
    }

    if (dtc->ref_frm_data) {
        free(dtc->ref_frm_data);
        dtc->ref_frm_data = NULL;
    }

    if (dtc->node_list) {
        free(dtc->node_list);
        dtc->node_list = NULL;
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
    dtc->video_stream_index = ret;

    /* create decoding context */
    dtc->dec_ctx = avcodec_alloc_context3(dec);
    if (!dtc->dec_ctx)
        return AVERROR(ENOMEM);
    avcodec_parameters_to_context(dtc->dec_ctx,
        dtc->fmt_ctx->streams[dtc->video_stream_index]->codecpar);

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
    const AVCodec *codec = NULL;
    const AVInputFormat *fmt = av_find_input_format("rawvideo");
    AVDictionary *options = NULL;

    av_dict_set(&options, "video_size", "1920x1080", 0);
    av_dict_set(&options, "pixel_format", "yuv420p", 0);

    if ((ret = avformat_open_input(&format_ctx, dtc->in_filename, fmt, &options)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }
    dtc->video_stream_index = 0;

    codec = avcodec_find_decoder_by_name("rawvideo");
    if (!codec) {
        fprintf(stderr, "Codec 'rawvideo' not found\n");
        return RET_NOK;
    }

    dec_ctx = avcodec_alloc_context3(codec);
    if (!dec_ctx) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return RET_NOK;
    }

    // 设置解码器上下文参数
    dec_ctx->codec_id = codec->id; //AV_CODEC_ID_RAWVIDEO
    dec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    dec_ctx->bit_rate = 400000; // 设置码率，对于rawvideo编解码器，这通常没有实际作用
    dec_ctx->width = dtc->width;
    dec_ctx->height = dtc->height;
    dec_ctx->pix_fmt = AV_PIX_FMT_YUV420P; // 设置像素格式为YUV420P
    dec_ctx->time_base = (AVRational){1, 25}; // 设置时间基，对于rawvideo编解码器，这通常没有实际作用
    dec_ctx->framerate = (AVRational){25, 1}; // 设置帧率，对于rawvideo编解码器，这通常没有实际作用
    dec_ctx->sample_aspect_ratio = (AVRational){1, 1}; // 设置采样率，对于rawvideo编解码器，这通常没有实际作用

    if (avcodec_open2(dec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return RET_NOK;
    }

    /* no need?? */
    if (avio_open(&format_ctx->pb, dtc->in_filename, AVIO_FLAG_READ) < 0) {
        fprintf(stderr, "Could not open input file\n");
        return RET_NOK;
    }

    dtc->fmt_ctx = format_ctx;
    dtc->dec_ctx = dec_ctx;

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
                           dtc->fmt_ctx->streams[dtc->video_stream_index]->time_base;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };

    dtc->filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !dtc->filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            dtc->dec_ctx->width, dtc->dec_ctx->height, dtc->dec_ctx->pix_fmt,
            time_base.num, time_base.den,
            dtc->dec_ctx->sample_aspect_ratio.num, dtc->dec_ctx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&dtc->buffersrc_ctx, buffersrc, "in",
                                       args, NULL, dtc->filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        goto end;
    }

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&dtc->buffersink_ctx, buffersink, "out",
                                       NULL, NULL, dtc->filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(dtc->buffersink_ctx, "pix_fmts", pix_fmts,
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
    outputs->filter_ctx = dtc->buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = dtc->buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(dtc->filter_graph, filters_descr,
                                    &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(dtc->filter_graph, NULL)) < 0)
        goto end;

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

static int build_filter_descr(DrawTextCtx *dtc)
{
    static const char *fontfile = "fontfile=/usr/share/fonts/truetype/freefont/FreeSans.ttf:"
                "fontsize=18:fontcolor=red:";
    char *dst = dtc->filter_descr;
    Node *node = dtc->node_list;
    int len = 0, idx, text_num = 0;
    int scale = 2;
    int madi_thd = 1;

    av_log(NULL, AV_LOG_INFO, "frame %d total text num %d\n", dtc->frm_cnt, dtc->node_num);

    len += snprintf(dst + len, MAX_STR_LEN, "scale=%d:%d,",
                    dtc->width * scale, dtc->height * scale);
    for (idx = 0; idx < dtc->node_num; idx++) {
        if ((len < MAX_FILTER_DESC_LEN - MAX_STR_LEN) && (node->madi > madi_thd)) {
            len += snprintf(dst + len, MAX_STR_LEN, "drawtext=%s", fontfile);
            len += snprintf(dst + len, MAX_STR_LEN, "text=%3d:x=%d:y=%d,",
                            node->madi, node->x * scale, node->y * scale);
            text_num++;

        }
        node++;
    }

    av_log(NULL, AV_LOG_INFO, "frame %d draw text num %d (madi > %d)\n",
           dtc->frm_cnt, text_num, madi_thd);

    return RET_OK;
}

static int build_filter(DrawTextCtx *dtc)
{
    RET ret = RET_OK;

    build_filter_descr(dtc);

    ret = init_filters(dtc, dtc->filter_descr);
    if (ret != RET_OK) {
        av_log(NULL, AV_LOG_ERROR, "init filters failed\n");
    }

    return ret;
}

static RET store_yuv_frame(DrawTextCtx *dtc, AVFrame *frame)
{
    int w = dtc->width;
    int h = dtc->height;
    int w32 = FFALIGN(dtc->width, 32);
    int h32 = FFALIGN(dtc->height, 32);
    int linesize = dtc->linesize;
    int i, k;

    dtc->ref_frm = dtc->cur_frm;

    for (i = 0; i < h32; i++) {
        memcpy(dtc->cur_frm + i * linesize,
               frame->data[0] + i * frame->linesize[0], w);

        if (w32 > w) {
            for (k = 0; k < w32 - w; k++)
                dtc->cur_frm[i * linesize + w + k] = dtc->cur_frm[i * linesize + w - 1];
        }
    }

    if (h32 > h) {
        for (k = 0; k < h32 - h; k++)
            memcpy(dtc->cur_frm + (h + k) * linesize,
                   dtc->cur_frm + (h - 1) * linesize, w32);
    }

    return RET_OK;
}

static int calc_block_madi(uint8_t *src, int stride, int blk_size)
{
    int ave, sum = 0, madi = 0;
    int i, j;


    for (i = 0; i < blk_size; i++) {
        for (j = 0; j < blk_size; j++) {
            sum += src[i * stride + j];
        }
    }

    ave = sum / (blk_size * blk_size);

    for (i = 0; i < blk_size; i++) {
        for (j = 0; j < blk_size; j++) {
            madi += abs(src[i * stride + j] - ave);
        }
    }

    return madi / (blk_size * blk_size);
}

static RET calc_frame_madi(DrawTextCtx *dtc)
{
    Node *node = dtc->node_list;
    int madi_size = dtc->blk_size_madi;
    uint8_t *src = NULL;
    int i, j;

    dtc->node_num = 0;
    for (i = 0; i < dtc->height; i += madi_size) {
        for (j = 0; j < dtc->width; j += madi_size) {
            src = dtc->cur_frm + i * dtc->linesize + j;
            node->x = j;
            node->y = i;
            node->blk_size_madi = madi_size;
            node->node_id = dtc->node_num++;
            node->madi = calc_block_madi(src, dtc->linesize, madi_size);
            av_log(NULL, AV_LOG_DEBUG, "(%d, %d) node_id %d madi %d\n",
                   node->x, node->y, node->node_id, node->madi);
            node++;
        }
    }

    return RET_OK;
}

static RET calc_madi_madp(DrawTextCtx *dtc, AVFrame *frame)
{
    store_yuv_frame(dtc, frame);

    calc_frame_madi(dtc);

    return RET_OK;
}

static void dump_frame(DrawTextCtx *dtc, const AVFrame *frame)
{
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

    /* read all packets */
    do {
        if ((ret = av_read_frame(dtc->fmt_ctx, packet)) < 0)
            break;

        if (packet->stream_index == dtc->video_stream_index) {
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

                calc_madi_madp(dtc, frame);
                if (build_filter(dtc) != RET_OK)
                    break;
                frame->pts = frame->best_effort_timestamp;

                /* push the decoded frame into the filtergraph */
                if (av_buffersrc_add_frame_flags(dtc->buffersrc_ctx,
                    frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
                    break;
                }

                /* pull filtered frames from the filtergraph */
                while (1) {
                    ret = av_buffersink_get_frame(dtc->buffersink_ctx, filt_frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    if (ret < 0)
                        goto end;

                    dump_frame(dtc, filt_frame);
                    av_frame_unref(filt_frame);
                }
                av_frame_unref(frame);
            }
        }

        av_packet_unref(packet);
        avfilter_graph_free(&dtc->filter_graph);
    } while (++dtc->frm_cnt < dtc->frames_to_filter);

    av_log(NULL, AV_LOG_INFO, "Filtered %d frames\n", dtc->frm_cnt);

end:
    avfilter_graph_free(&dtc->filter_graph);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    av_packet_free(&packet);
    draw_txt_filter_deinit(dtc);

    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
    }

    return ret;
}