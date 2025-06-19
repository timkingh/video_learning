#include "parse_hview_av1_stream.h"

static const uint32_t kMaxFrameNum = 10000;
static const uint32_t flag0 = (0x00 << 24) | (0x15 << 16) | (0x11 << 8) | 0x10;
static const uint32_t flag1 = (0x01 << 24) | (0x15 << 16) | (0x11 << 8) | 0x10;
static const uint32_t flag2 = (0x02 << 24) | (0x15 << 16) | (0x11 << 8) | 0x10;

typedef struct {
    uint32_t flag_pos[kMaxFrameNum];
    uint32_t flag_len[kMaxFrameNum];
    uint32_t strm_pos[kMaxFrameNum];
    uint32_t strm_len[kMaxFrameNum];
    uint32_t frame_pos[kMaxFrameNum];
    uint32_t frame_type[kMaxFrameNum]; /* 1 - key frame, 2 - inter frame */
    uint32_t flag_cnt;
    uint32_t frame_cnt;
    uint32_t write_frm_num;
    uint8_t *src_buf;
    uint8_t *dst_buf;
    uint32_t src_len;
    uint32_t dst_len;
    uint32_t read_len;
    uint32_t write_len;
	CalcCtx *calc_ctx;
    FILE *fp_in;
    FILE *fp_out;
} ParseCtx;

uint8_t ivf_header[32] = {
    0x44, 0x4B, 0x49, 0x46, 0x00, 0x00, 0x20, 0x00,
    0x41, 0x56, 0x30, 0x31, 0x80, 0x07, 0x38, 0x04, /* 0x780 - 1920, 0x438 - 1080 */
    0x1E, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t frm_header[12] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,    0x00, 0x00
};

static RET parse_hview_av1_stream_init(ParseCtx *ps_ctx, CalcCtx *calc_ctx)
{
    const char *input_file = calc_ctx->input.c_str();
    const char *output_file = calc_ctx->input_cmp.c_str();

    ps_ctx->fp_in = fopen(input_file, "rb");
    if (ps_ctx->fp_in == NULL) {
        perror("fopen input");
        return RET_NOK;
    }

    ps_ctx->fp_out = fopen(output_file, "w+");
    if (ps_ctx->fp_out == NULL) {
        perror("fopen output");
        return RET_NOK;
    }

    ps_ctx->src_len = 256 * 1024 * 1024;
    ps_ctx->dst_len = 2 * 1024 * 1024;

    ps_ctx->src_buf = (uint8_t *)calloc(ps_ctx->src_len, sizeof(uint8_t));
    if (ps_ctx->src_buf == NULL) {
        perror("malloc src_buf");
        return RET_NOK;
    }

    ps_ctx->dst_buf = (uint8_t *)calloc(ps_ctx->dst_len, sizeof(uint8_t));
    if (ps_ctx->dst_buf == NULL) {
        perror("malloc dst_buf");
        return RET_NOK;
    }

    ps_ctx->write_frm_num = calc_ctx->frames;
	ps_ctx->calc_ctx = calc_ctx;

    return RET_OK;
}

static void parse_hview_av1_stream_deinit(ParseCtx *ps_ctx)
{
    FPCLOSE(ps_ctx->fp_in);
    FPCLOSE(ps_ctx->fp_out);
    FREE(ps_ctx->src_buf);
    FREE(ps_ctx->dst_buf);
}

static RET parse_hview_av1_stream_read(ParseCtx *ps_ctx)
{
    uint8_t *buf = ps_ctx->src_buf;
    uint32_t len = ps_ctx->src_len;
    uint32_t read_len = 0;
    while (read_len < len) {
        uint32_t read_now = fread(buf + read_len, 1, len - read_len, ps_ctx->fp_in);
        if (read_now == 0) {
            break;
        }
        read_len += read_now;
    }

    ps_ctx->read_len = read_len;
    printf("read_len = %d\n", read_len);

    return RET_OK;
}

#if 0
static RET parse_hview_stream_flag(ParseCtx *ps_ctx)
{
    uint8_t *buf = ps_ctx->src_buf;
    uint32_t *flag_buf = NULL;
    uint32_t parse_len = 0;
    uint32_t flag_pos = 0;
    uint32_t flag_len = 0;

    while(parse_len < ps_ctx->read_len) {
        flag_buf = (uint32_t *)(buf + parse_len);
        if ((*flag_buf == flag0 || *flag_buf == flag1 || *flag_buf == flag2) &&
            ps_ctx->flag_cnt < kMaxFrameNum){
            flag_pos = parse_len;
            parse_len += 4;
            ps_ctx->flag_pos[ps_ctx->flag_cnt] = flag_pos;
            ps_ctx->flag_cnt++;
        }
        parse_len++;
    }

    printf("flag_cnt = %d\n", ps_ctx->flag_cnt);
    for(uint32_t i = 0; i < ps_ctx->flag_cnt - 1; i++) {
        flag_len = ps_ctx->flag_pos[i+1] - ps_ctx->flag_pos[i];
        ps_ctx->flag_len[i] = flag_len;
        // printf("i %d flag_pos = 0x%x, flag_len = 0x%x\n", i, ps_ctx->flag_pos[i], flag_len);
    }

    return RET_OK;
}

static RET parse_hview_stream_frame(ParseCtx *ps_ctx)
{
    const uint16_t td_flag = (0x00 << 8) | 0x12;
    uint16_t *td_buf = NULL;

    for(uint32_t i = 0; i < ps_ctx->flag_cnt - 1; i++) {
        for (uint32_t pos = 0; pos < ps_ctx->flag_len[i]; pos++) {
            td_buf = (uint16_t *)(ps_ctx->src_buf + ps_ctx->flag_pos[i] + pos);

            /* 0x818 may be changed(20240528)
             * pos >= 24: skip the first 24 bytes(AVI info from h.view)
             */
            if ((*td_buf == td_flag) && (ps_ctx->flag_len[i] != 0x818) &&
                (pos >= 24)) {
                ps_ctx->strm_pos[ps_ctx->frame_cnt] = ps_ctx->flag_pos[i] + pos;
                ps_ctx->strm_len[ps_ctx->frame_cnt] = ps_ctx->flag_pos[i+1] - ps_ctx->flag_pos[i] - pos;
                ps_ctx->frame_cnt++;
                break;
            }
        }
    }

    printf("frame_cnt = %d\n", ps_ctx->frame_cnt);
    for(uint32_t i = 0; i < ps_ctx->frame_cnt; i++) {
        // printf("strm_pos = 0x%x, strm_len = 0x%x\n", ps_ctx->strm_pos[i], ps_ctx->strm_len[i]);
    }

    return RET_OK;
}
#endif

static RET parse_hview_stream_frame_v2(ParseCtx *ps_ctx)
{
    uint8_t *buf = ps_ctx->src_buf;
    uint32_t *flag_buf = NULL;
    uint32_t *flag_end = NULL;
    uint32_t *frame_size = NULL;
    uint32_t parse_len = 0;

    while(parse_len < ps_ctx->read_len) {
        /* hview avi info: 24 bytes
         * 10 11 15 01 c9 a2 00 00 ab ce da ba 8f 01 00 00 00 00 00 00 00 00 00 00
         * 10 11 15 02 f9 20 01 00 ba ce da ba 8f 01 00 00 00 00 00 00 00 00 00 00
         */
        flag_buf = (uint32_t *)(buf + parse_len);
        frame_size = (uint32_t *)(buf + parse_len + 4);
        flag_end = (uint32_t *)(buf + parse_len + 20);

        /* flag1: I frame ?? flag2: P frame ?? (20240528) */
        if ((*flag_buf == flag1 || *flag_buf == flag2) &&
            (*flag_end == 0) && (ps_ctx->flag_cnt < kMaxFrameNum)){
            ps_ctx->strm_pos[ps_ctx->frame_cnt] = parse_len + 24;
            ps_ctx->strm_len[ps_ctx->frame_cnt] = *frame_size;
            ps_ctx->frame_type[ps_ctx->frame_cnt] = (*flag_buf == flag1) ? 1 : 2;
            parse_len += 24;
            ps_ctx->frame_cnt++;
        }
        parse_len++;
    }

    printf("frame_cnt = %d\n", ps_ctx->frame_cnt);
    for(uint32_t i = 0; i < ps_ctx->frame_cnt; i++) {
        // printf("frame %d type %d strm_pos = 0x%x, strm_len = 0x%x\n",
        //        i, ps_ctx->frame_type[i], ps_ctx->strm_pos[i], ps_ctx->strm_len[i]);
    }

    return RET_OK;
}

static RET parse_hview_av1_stream_write(ParseCtx *ps_ctx)
{
    uint8_t *buf = ps_ctx->src_buf;
	uint16_t *buf_wh = NULL;
	uint32_t *frm_num = NULL;
	uint32_t *fps = NULL;
    uint32_t write_len = 0;

	buf_wh = (uint16_t *)(ivf_header + 12);
	*buf_wh = (uint16_t)(ps_ctx->calc_ctx->width & 0xffff);
	*(buf_wh + 1) = (uint16_t)(ps_ctx->calc_ctx->height & 0xffff);

	fps = (uint32_t *)(ivf_header + 16);
	*fps = (uint32_t)(ps_ctx->calc_ctx->framerate & 0xffffffff);

	frm_num = (uint32_t *)(ivf_header + 24);
	*frm_num = ps_ctx->frame_cnt;
    write_len = fwrite(ivf_header, 1, 32, ps_ctx->fp_out);

    for(uint32_t i = 0; i < ps_ctx->frame_cnt; i++) {
        uint32_t *frm_size = (uint32_t *)(frm_header);
        uint64_t *timestamp = (uint64_t *)(frm_header + 4);

        *frm_size = ps_ctx->strm_len[i];
        *timestamp = i;
        write_len += fwrite(frm_header, 1, 12, ps_ctx->fp_out);

        ps_ctx->frame_pos[i] = write_len;
        // printf("frame %d pos 0x%x len 0x%x\n", i, write_len, ps_ctx->strm_len[i]);

        write_len += fwrite(buf + ps_ctx->strm_pos[i], 1, ps_ctx->strm_len[i], ps_ctx->fp_out);

        if ((i + 1) >= ps_ctx->write_frm_num) {
            break;
        }
    }

    ps_ctx->write_len = write_len;
    printf("write_len = %d\n", write_len);

    return RET_OK;
}

RET parse_hview_av1_stream(CalcCtx *ctx)
{
    RET ret = RET_OK;
    ParseCtx ps_ctx;
    memset(&ps_ctx, 0, sizeof(ParseCtx));

    ret = parse_hview_av1_stream_init(&ps_ctx, ctx);
    if (ret != RET_OK) {
        printf("parse_hview_av1_stream_init failed\n");
        goto fail;
    }

    ret = parse_hview_av1_stream_read(&ps_ctx);
    if (ret != RET_OK) {
        printf("parse_hview_av1_stream_read failed\n");
        goto fail;
    }

#if 0
    ret = parse_hview_stream_flag(&ps_ctx);
    if (ret != RET_OK) {
        printf("parse_hview_stream_flag failed\n");
        goto fail;
    }

    ret = parse_hview_stream_frame(&ps_ctx);
    if (ret != RET_OK) {
        printf("parse_hview_stream_frame failed\n");
        goto fail;
    }
#else
    ret = parse_hview_stream_frame_v2(&ps_ctx);
    if (ret != RET_OK) {
        printf("parse_hview_stream_frame_v2 failed\n");
        goto fail;
    }
#endif

    ret = parse_hview_av1_stream_write(&ps_ctx);
    if (ret != RET_OK) {
        printf("parse_hview_av1_stream_write failed\n");
        goto fail;
    }

fail:
    parse_hview_av1_stream_deinit(&ps_ctx);

    return ret;
}