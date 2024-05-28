#include "parse_hview_av1_stream.h"

typedef struct {
    uint8_t *src_buf;
    uint8_t *dst_buf;
    uint32_t src_len;
    uint32_t dst_len;
	uint32_t read_len;
    uint32_t write_len;
    FILE *fp_in;
    FILE *fp_out;
} ParseCtx;

static RET parse_hview_av1_stream_init(ParseCtx *parse_ctx, CalcCtx *calc_ctx)
{
    const char *input_file = calc_ctx->input.c_str();
    const char *output_file = calc_ctx->input_cmp.c_str();

    parse_ctx->fp_in = fopen(input_file, "rb");
    if (parse_ctx->fp_in == NULL) {
        perror("fopen input");
        return RET_NOK;
    }

    parse_ctx->fp_out = fopen(output_file, "rb");
    if (parse_ctx->fp_out == NULL) {
        perror("fopen org");
        return RET_NOK;
    }

	parse_ctx->src_len = 256 * 1024 * 1024;
    parse_ctx->dst_len = 256 * 1024 * 1024;

    parse_ctx->src_buf = (uint8_t *)calloc(parse_ctx->src_len, sizeof(uint8_t));
    if (parse_ctx->src_buf == NULL) {
        perror("malloc src_buf");
        return RET_NOK;
    }

    parse_ctx->dst_buf = (uint8_t *)calloc(parse_ctx->dst_len, sizeof(uint8_t));
    if (parse_ctx->dst_buf == NULL) {
        perror("malloc dst_buf");
        return RET_NOK;
	}

    return RET_OK;
}

static void parse_hview_av1_stream_deinit(ParseCtx *parse_ctx)
{
    FPCLOSE(parse_ctx->fp_in);
    FPCLOSE(parse_ctx->fp_out);
	FREE(parse_ctx->src_buf);
    FREE(parse_ctx->dst_buf);
}

static RET parse_hview_av1_stream_read(ParseCtx *parse_ctx)
{
	uint8_t *buf = parse_ctx->src_buf;
    uint32_t len = parse_ctx->src_len;
    uint32_t read_len = 0;
    while (read_len < len) {
        uint32_t read_now = fread(buf + read_len, 1, len - read_len, parse_ctx->fp_in);
        if (read_now == 0) {
            break;
        }
        read_len += read_now;
    }

	parse_ctx->read_len = read_len;
	printf("read_len = %d\n", read_len);

    return RET_OK;
}

RET parse_hview_av1_stream(CalcCtx *ctx)
{
    RET ret = RET_OK;
    ParseCtx parse_ctx;
    memset(&parse_ctx, 0, sizeof(ParseCtx));

    ret = parse_hview_av1_stream_init(&parse_ctx, ctx);
    if (ret != RET_OK) {
		printf("parse_hview_av1_stream_init failed\n");
        goto fail;
    }

	ret = parse_hview_av1_stream_read(&parse_ctx);
    if (ret != RET_OK) {
		printf("parse_hview_av1_stream_read failed\n");
        goto fail;
    }

fail:
    parse_hview_av1_stream_deinit(&parse_ctx);

    return ret;
}