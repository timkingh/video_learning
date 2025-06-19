#define _CRT_SECURE_NO_WARNINGS
#include "calc_psnr.h"

#define PIXEL_MAX 255

static double x264_psnr(double sqe, double size)
{
	double mse = sqe / (PIXEL_MAX * PIXEL_MAX * size);
	if (mse <= 0.0000000001)  /* Max 100dB */
		return 100;

	return -10.0 * log10(mse);
}

RET calc_psnr(CalcCtx *ctx)
{
	int w = ctx->width;
	int h = ctx->height;
	int frame_num = ctx->frames;
	uint32_t frame_size = w * h * 3 / 2;
	const char *input_file = ctx->input.c_str();
	const char *output_file = ctx->input_cmp.c_str();
	unsigned char *y_org, *y_buf;
	int j, i;
	size_t ret_len_org, ret_len_chg;
	int real_frm_cnt = 0;
	long long ssd;
	long long ssd_global = 0;
	double psnr = 0;
	FILE *fp_yuv_in;
	FILE *fp_yuv_org;
	int64_t start_time = time_mdate();
	int64_t end_time;

	fp_yuv_in = fopen(input_file, "rb");
	if (fp_yuv_in == NULL) {
		perror("fopen input");
		return RET_NOK;
	}

	fp_yuv_org = fopen(output_file, "rb");
	if (fp_yuv_org == NULL) {
		perror("fopen org");
		return RET_NOK;
	}

	y_buf = (unsigned char *)malloc(w * h * 2);
	if (y_buf == NULL) {
		printf("malloc y_buf failed\n");
		return RET_NOK;
	}

	y_org = (unsigned char *)malloc(w * h * 2);
	if (y_org == NULL) {
		printf("malloc y_org failed\n");
		return RET_NOK;
	}

	if (ctx->log_frames == 0) {
		ctx->log_frames = 1;
	}

	real_frm_cnt = 0;
	for (i = 0; i < frame_num; i++)
	{
		ret_len_org = fread(y_buf, 1, frame_size, fp_yuv_in);
		ret_len_chg = fread(y_org, 1, frame_size, fp_yuv_org);
		if (0 == ret_len_chg || 0 == ret_len_org || ret_len_chg < frame_size || ret_len_org < frame_size)
		{
			break;
		}

		if ((real_frm_cnt % ctx->log_frames) == (ctx->log_frames - 1)) {
			printf("fread frame %03d\n", real_frm_cnt);
		}

		real_frm_cnt++;
		ssd = 0;
		for (j = 0; j < (int)frame_size; j++)
			ssd += (y_buf[j] - y_org[j])*(y_buf[j] - y_org[j]);

		ssd_global += ssd;
	}
	printf("end of frame %d\n", real_frm_cnt);

	psnr = x264_psnr((double)ssd_global / real_frm_cnt, frame_size);
	end_time = time_mdate();

	FILE *fp = fopen(ctx->output.c_str(), "ab");
	FPRINT(fp, "%s", input_file);
	FPRINT(fp, "      %lf\n", psnr);
	FPCLOSE(fp);
	printf("%s", input_file);
	printf(" elapsed %.2fs, global psnr = %lf  \n", (float)(end_time - start_time) / 1000000, psnr);
	FPCLOSE(fp_yuv_in);
	FPCLOSE(fp_yuv_org);
	free(y_buf);
	free(y_org);

	return RET_OK;
}