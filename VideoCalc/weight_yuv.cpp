#define _CRT_SECURE_NO_WARNINGS
#include "weight_yuv.h"

typedef struct {
	int32_t frames;
	int32_t plane;
	int32_t scale;
	int32_t denom;
	int32_t offset;
}weight_t;

static RET handle_weight(uint8_t *buf, FILE *fp, uint32_t len, weight_t &w, uint8_t *buf_out)
{
	size_t read_len;
	uint32_t j;
	int32_t value;
	int32_t weight = w.scale, denom = w.denom, offset = w.offset;
	uint32_t div_offset = (denom == 0) ? 0 : 1 << (denom - 1);

	read_len = fread(buf, 1, len, fp);
	if (0 == read_len || read_len < len) {
		printf("fread finished!\n");
		return RET_NOK;
	}

	for (j = 0; j < len; j++) {
		value = ((weight * buf[j] + div_offset) >> denom) + offset;
		buf_out[j] = value < 0 ? 0 : ((value > 255) ? 255 : value);
	}

	return RET_OK;
}

static RET output_weighted_yuv(uint8_t *buf, FILE *fp, uint32_t len)
{
	size_t w_len;
	w_len = fwrite(buf, 1, len, fp);
	if (w_len != len) {
		perror("output weighted yuv");
	}

	return RET_OK;
}

static RET output_combo_yuv(uint8_t *buf0, uint8_t *buf1, FILE *fp, uint32_t len)
{
	size_t w_len;
	w_len = fwrite(buf0, 1, len, fp);
	if (w_len != len) {
		perror("output original yuv");
	}

	w_len = fwrite(buf1, 1, len, fp);
	if (w_len != len) {
		perror("output weighted yuv");
	}

	return RET_OK;
}

RET weight_yuv(CalcCtx *ctx)
{
	uint32_t frame_size = ctx->width * ctx->height;
	const char *input_file = ctx->input.c_str();
	const char *output_file = ctx->input_cmp.c_str();
	const char *combo_file = ctx->out_yuv.c_str();
    ifstream param(ctx->output.c_str());
	uint8_t *buf, *buf_out;
	uint32_t j, i;
	uint32_t y_len = ctx->width * ctx->height;
	uint32_t u_len = y_len / 4;
	FILE *fp_yuv_in;
	FILE *fp_out, *fp_combo;
	int64_t start_time = time_mdate();
	int64_t end_time;
	weight_t w;
	char lines[512];

	fp_yuv_in = fopen(input_file, "rb");
	if (fp_yuv_in == NULL) {
		perror("fopen input");
		return RET_NOK;
	}

	fp_out = fopen(output_file, "wb");
	if (fp_out == NULL) {
		perror("fopen org");
		return RET_NOK;
	}	

	fp_combo = fopen(combo_file, "wb");
	if (fp_combo == NULL) {
		perror("fopen combo");
		return RET_NOK;
	}

	buf = (uint8_t *)malloc(ctx->width * ctx->height * 2);
	if (buf == NULL) {
		printf("malloc buf failed\n");
		return RET_NOK;
	}
	
	buf_out = (uint8_t *)malloc(ctx->width * ctx->height * 2);
	if (buf_out == NULL) {
		printf("malloc buf_out failed\n");
		return RET_NOK;
	}

	for (i = 0; i < ctx->frames; i++) {			
		uint8_t *p0 = buf;
		uint8_t *p1 = buf_out;

		for (j = 0; j < 3; j++) {
			uint32_t len = (j == 0) ? y_len : u_len;
			
		    if (param.getline(lines, 512)) {
		        int match_cnt = SSCANF(lines, "frame %d plane %d minscale %d mindenom %d minoff %d",
		                               &w.frames, &w.plane, &w.scale, &w.denom, &w.offset);
		        if (match_cnt != 5) {
					printf("fetch param failed! match_cnt %d\n", match_cnt);
		        }
		    }
			
			if (RET_OK != handle_weight(p0, fp_yuv_in, len, w, p1)) {
				printf("frame %d fread %s finished\n", i, (j == 0) ? "Luma" : ((j == 1) ? "Chroma U" : "Chroma V"));
				break;
			}

			p0 += len;
			p1 += len;
		}
		
		output_weighted_yuv(buf_out, fp_out, y_len + u_len * 2);
		output_combo_yuv(buf, buf_out, fp_combo, y_len + u_len * 2);
	}

	end_time = time_mdate();

	printf("weight frame %d elapsed %.2fs\n", ctx->frames, (float)(end_time - start_time) / 1000000);
	FPCLOSE(fp_yuv_in);
	FPCLOSE(fp_out);
	FPCLOSE(fp_combo);
	free(buf);
	free(buf_out);

	return RET_OK;
}
