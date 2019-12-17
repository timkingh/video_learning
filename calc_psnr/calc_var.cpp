#define _CRT_SECURE_NO_WARNINGS
#include "calc_var.h"

static RET calc_ave(uint8_t *buf, FILE *fp, uint32_t len, float *ave)
{
	uint32_t read_len, j;
	double sum = 0;

	read_len = fread(buf, 1, len, fp);
	if (0 == read_len || read_len < len) {
		printf("fread finished!\n");
		return RET_NOK;
	}

	for (j = 0; j < len; j++) {
		sum += buf[j];
	}

	*ave = (sum) / len;

	return RET_OK;
}

static float calc_variance(uint8_t *buf, uint32_t len, float ave)
{
	double sum_square = 0;
	for (uint32_t j = 0; j < len; j++) {
		sum_square += (buf[j] - ave) * (buf[j] - ave);
	}

	return (sum_square + len / 2) / len;
}

RET calc_var(CalcCtx *ctx)
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
	float *var_buf = NULL, *mean_buf = NULL;
	uint32_t var_idx = 0, mean_idx = 0;

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

	for (i = 0; i < ctx->frames; i++) {
		for (j = 0; j < 3; j++) {
			if (RET_OK != calc_ave(buf, fp_yuv_in, (j == 0) ? y_len : u_len, &average[j])) {
				printf("frame %d fread %s finished\n", i, (j == 0) ? "Luma" : ((j == 1) ? "Chroma U" : "Chroma V"));
				break;
			}
			variance[j] = calc_variance(buf, (j == 0) ? y_len : u_len, average[j]);
			fprintf(fp_out, "frame %d plane %d ave %f var %f\n", i, j, average[j], variance[j]);

			var_buf[var_idx++] = variance[j];
			mean_buf[mean_idx++] = average[j];
		}
	}

	if (ctx->var_ratio_flg) {
		float ratio[3] = { 0 };
		float mean_ratio[3] = { 0 };
		for (i = 0; i < ctx->frames - 1; i++) {
			ratio[0] = sqrtf(var_buf[(i + 1) * 3 + 0] / var_buf[i * 3 + 0]);
			ratio[1] = sqrtf(var_buf[(i + 1) * 3 + 1] / var_buf[i * 3 + 1]);
			ratio[2] = sqrtf(var_buf[(i + 1) * 3 + 2] / var_buf[i * 3 + 2]);
			FPRINT(fp_out, "frame %d ratio %f %f %f\n", i + 1, ratio[0], ratio[1], ratio[2]);

			mean_ratio[0] = mean_buf[(i + 1) * 3 + 0] / mean_buf[i * 3 + 0];
			mean_ratio[1] = mean_buf[(i + 1) * 3 + 1] / mean_buf[i * 3 + 1];
			mean_ratio[2] = mean_buf[(i + 1) * 3 + 2] / mean_buf[i * 3 + 2];
			FPRINT(fp_out, "frame %d mean_ratio %f %f %f\n", i + 1, mean_ratio[0], mean_ratio[1], mean_ratio[2]);
		}
	}

	end_time = time_mdate();

	printf("calc frame %d elapsed %.2fs\n", ctx->frames, (float)(end_time - start_time) / 1000000);
	FPCLOSE(fp_yuv_in);
	FPCLOSE(fp_out);
	free(buf);
	free(var_buf);
	free(mean_buf);

	return RET_OK;
}