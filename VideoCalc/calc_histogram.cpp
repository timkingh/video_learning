#define _CRT_SECURE_NO_WARNINGS
#include "calc_var.h"

static RET calc_ave(uint8_t *buf, FILE *fp, uint32_t len, float *ave)
{
	size_t read_len;
	uint32_t j;
	double sum = 0;

	read_len = fread(buf, 1, len, fp);
	if (0 == read_len || read_len < len) {
		printf("fread finished!\n");
		return RET_NOK;
	}

	for (j = 0; j < len; j++) {
		sum += buf[j];
	}

	*ave = (float)((sum) / len);

	return RET_OK;
}

static float calc_variance(uint8_t *buf, uint32_t len, float ave)
{
	double sum_square = 0;
	for (uint32_t j = 0; j < len; j++) {
		sum_square += (buf[j] - ave) * (buf[j] - ave);
	}

	return (float)((sum_square + len / 2) / len);
}

static uint64_t calc_madi(uint8_t *buf, uint32_t len, uint8_t ave)
{
	uint64_t madi = 0;
	for (uint32_t j = 0; j < len; j++) {
		madi += abs(buf[j] - ave);
	}

	return madi;
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
	uint64_t madi[3] = { 0 };
	float *var_buf = NULL, *mean_buf = NULL;
	uint64_t *mad_buf = NULL;
	uint32_t var_idx = 0, mean_idx = 0, mad_idx = 0;

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
			if (RET_OK != calc_ave(buf, fp_yuv_in, (j == 0) ? y_len : u_len, &average[j])) {
				printf("frame %d fread %s finished\n", i, (j == 0) ? "Luma" : ((j == 1) ? "Chroma U" : "Chroma V"));
				break;
			}
			variance[j] = calc_variance(buf, (j == 0) ? y_len : u_len, average[j]);

			madi[j] = calc_madi(buf, (j == 0) ? y_len : u_len, (uint8_t)average[j]);
			fprintf(fp_out, "frame %d plane %d ave %f var %f madi %d\n", i, j, average[j], variance[j], madi[j]);

			var_buf[var_idx++] = variance[j];
			mean_buf[mean_idx++] = average[j];
			mad_buf[mad_idx++] = madi[j];
		}
	}

	if (ctx->var_ratio_flg) {
		float var_ratio[3] = { 0 };
		float mean_ratio[3] = { 0 };
		float mad_ratio[3] = { 0 };
		for (i = 0; i < ctx->frames - 1; i++) {
			for (j = 0; j < 3; j++) {
				float numer = var_buf[(i + 1) * 3 + j] + !var_buf[i * 3 + j];
				float denom = var_buf[i * 3 + j]       + !var_buf[i * 3 + j];
				var_ratio[j] = sqrtf(numer / denom);

				numer = mean_buf[(i + 1) * 3 + j] + !mean_buf[i * 3 + j];
				denom = mean_buf[i * 3 + j]       + !mean_buf[i * 3 + j];
				mean_ratio[j] = numer / denom;

				numer = (float)mad_buf[(i + 1) * 3 + j] + !mad_buf[i * 3 + j];
				denom = (float)mad_buf[i * 3 + j]       + !mad_buf[i * 3 + j];
				mad_ratio[j] = numer / denom;
				FPRINT(fp_out, "frame %d plane %d mean_ratio %f var_ratio %f mad_ratio %f\n", i + 1, j, 
								mean_ratio[j], var_ratio[j], mad_ratio[j]);
			}
		}
	}

	end_time = time_mdate();

	printf("calc frame %d elapsed %.2fs\n", ctx->frames, (float)(end_time - start_time) / 1000000);
	FPCLOSE(fp_yuv_in);
	FPCLOSE(fp_out);
	free(buf);
	free(var_buf);
	free(mean_buf);
	free(mad_buf);

	return RET_OK;
}
