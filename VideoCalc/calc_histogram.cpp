#define _CRT_SECURE_NO_WARNINGS
#include "calc_histogram.h"

#define MAX_HIST_LEN  (256)

static RET calc_hist(uint8_t *buf, FILE *fp, uint32_t len, vector<int> &hist)
{
	size_t read_len;
	uint32_t j;
	int32_t value;
	hist.clear();
	hist.resize(MAX_HIST_LEN);

	read_len = fread(buf, 1, len, fp);
	if (0 == read_len || read_len < len) {
		printf("fread finished!\n");
		return RET_NOK;
	}

	for (j = 0; j < len; j++) {
		value = buf[j];
		value = value < 0 ? 0 : ((value > 255) ? 255 : value);
		hist[value]++;
	}

	return RET_OK;
}

static void disp_hist(vector<int> &hist, FILE *fp)
{
	for (int i = 0; i < hist.size(); i++) {
		FPRINT(fp, "pixel %3d --- %8d\n", i, hist[i]);
	}
}

RET calc_histogram(CalcCtx *ctx)
{
	uint32_t frame_size = ctx->width * ctx->height;
	const char *input_file = ctx->input.c_str();
	const char *output_file = ctx->output.c_str();
	uint8_t *buf;
	uint32_t j, i;
	uint32_t y_len = ctx->width * ctx->height;
	uint32_t u_len = y_len / 4;
	FILE *fp_yuv_in;
	FILE *fp_out;
	int64_t start_time = time_mdate();
	int64_t end_time;
	float average[3] = { 0 };
	float variance[3] = { 0 };	
	uint64_t madi[3] = { 0 };
	float *var_buf = NULL, *mean_buf = NULL;
	uint64_t *mad_buf = NULL;
	uint32_t var_idx = 0, mean_idx = 0, mad_idx = 0;
	vector<int> hist;

	fp_yuv_in = fopen(input_file, "rb");
	if (fp_yuv_in == NULL) {
		perror("fopen input");
		return RET_NOK;
	}

	fp_out = fopen(output_file, "ab");
	if (fp_out == NULL) {
		perror("fopen org");
		return RET_NOK;
	}

	buf = (uint8_t *)malloc(ctx->width * ctx->height * 2);
	if (buf == NULL) {
		printf("malloc buf failed\n");
		return RET_NOK;
	}

	var_buf = (float *)malloc(ctx->frames * 3 * sizeof(float));
	if (var_buf == NULL) {
		printf("malloc var_buf failed\n");
		return RET_NOK;
	}

	mean_buf = (float *)malloc(ctx->frames * 3 * sizeof(float));
	if (mean_buf == NULL) {
		printf("malloc mean_buf failed\n");
		return RET_NOK;
	}

	mad_buf = (uint64_t *)malloc(ctx->frames * 3 * sizeof(uint64_t));
	if (mad_buf == NULL) {
		printf("malloc mad_buf failed\n");
		return RET_NOK;
	}

	for (i = 0; i < ctx->frames; i++) {
		for (j = 0; j < 3; j++) {
			if (RET_OK != calc_hist(buf, fp_yuv_in, (j == 0) ? y_len : u_len, hist)) {
				printf("frame %d fread %s finished\n", i, (j == 0) ? "Luma" : ((j == 1) ? "Chroma U" : "Chroma V"));
				break;
			}

			ctx->hist_org.push_back(hist);
		}
	}

	assert(ctx->frames * 3 <= ctx->hist_org.size());

	for (i = 0; i < ctx->frames; i++) {
		for (j = 0; j < 3; j++) {
			vector<int> vec = ctx->hist_org[i * 3 + j];
			FPRINT(fp_out, "frame %3d plane %d\n", i, j);
			disp_hist(vec, fp_out);
		}
	}

	end_time = time_mdate();

	printf("calc histogram %d elapsed %.2fs\n", ctx->frames, (float)(end_time - start_time) / 1000000);
	FPCLOSE(fp_yuv_in);
	FPCLOSE(fp_out);
	free(buf);
	free(var_buf);
	free(mean_buf);
	free(mad_buf);

	return RET_OK;
}
