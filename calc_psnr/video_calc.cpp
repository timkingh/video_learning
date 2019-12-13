#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#include <stdint.h>
#if _WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif
#include "getopt.hpp"

#define PIXEL_MAX 255

using namespace std;

#define     FPRINT(fp, ...)  { if (fp) { fprintf(fp, ## __VA_ARGS__);} }
#define     FPCLOSE(fp, ...) { if (fp) { fclose(fp); fp = NULL;} }

enum MODE {
	CALC_PSNR = 0,
	CALC_VAR = 1,
};

enum RET {
	RET_NOK = -1,
	RET_OK = 0,
};

typedef struct {
    string input;
    string input_cmp;
    string output;
    uint32_t width;
    uint32_t height;
    uint32_t frames;
    uint32_t mode;
    uint32_t var_ratio_flg;
} CalcCtx;

int64_t time_mdate(void)
{
#if _WIN32
    struct timeb tb;
    ftime(&tb);
    return ((int64_t)tb.time * 1000 + (int64_t)tb.millitm) * 1000;
#else
    struct timeval tv_date;
    gettimeofday(&tv_date, NULL);
    return (int64_t)tv_date.tv_sec * 1000000 + (int64_t)tv_date.tv_usec;
#endif
}

static double x264_psnr(double sqe, double size)
{
	double mse = sqe / (PIXEL_MAX * PIXEL_MAX * size);
	if (mse <= 0.0000000001)  /* Max 100dB */
		return 100;

	return -10.0 * log10(mse);
}

static RET calc_psnr(CalcCtx *ctx)
{
    int w = ctx->width;
    int h = ctx->height;
	int frame_num = ctx->frames;   
    int frame_size = w * h * 3 / 2;
	const char *input_file = ctx->input.c_str();
	const char *output_file = ctx->input_cmp.c_str();
    unsigned char *y_org, *y_buf;
	int j, i;
	int ret_len_org, ret_len_chg;
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

	real_frm_cnt = 0;
	for (i = 0; i < frame_num; i++)
	{
		ret_len_org = fread(y_buf, 1, frame_size, fp_yuv_in);
		ret_len_chg = fread(y_org, 1, frame_size, fp_yuv_org);
		if (0 == ret_len_chg || 0 == ret_len_org || ret_len_chg < frame_size || ret_len_org < frame_size)
		{
			break;
		}
        printf("fread frame %03d\n", real_frm_cnt);

		real_frm_cnt++;
		ssd = 0;
		for (j = 0; j < frame_size; j++)
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

static RET calc_var(CalcCtx *ctx)
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
	float average[3] = {0};
	float variance[3] = {0};
	float *var_buf = NULL;
	uint32_t var_idx = 0;

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

	for (i = 0; i < ctx->frames; i++) {
		for (j = 0; j < 3; j++) {
			if (RET_OK != calc_ave(buf, fp_yuv_in, (j == 0) ? y_len : u_len, &average[j])) {
				printf("frame %d fread %s finished\n", i, (j == 0) ? "Luma" : ((j == 1) ? "Chroma U" : "Chroma V"));
				break;
			}
			variance[j] = calc_variance(buf, (j == 0) ? y_len : u_len, average[j]);
			fprintf(fp_out, "frame %d plane %d ave %f var %f\n", i, j, average[j], variance[j]);

			var_buf[var_idx++] = variance[j];
		}
	}

	if (ctx->var_ratio_flg) {
		float ratio[3] = {0};
		for (i = 0; i < ctx->frames - 1; i++) {
			ratio[0] = sqrtf(var_buf[(i + 1) * 3 + 0] / var_buf[i * 3 + 0]);
			ratio[1] = sqrtf(var_buf[(i + 1) * 3 + 1] / var_buf[i * 3 + 1]);
			ratio[2] = sqrtf(var_buf[(i + 1) * 3 + 2] / var_buf[i * 3 + 2]);
			FPRINT(fp_out, "frame %d ratio %f %f %f\n", i + 1, ratio[0], ratio[1], ratio[2]);
		}
	}
	
    end_time = time_mdate();

	printf("calc frame %d elapsed %.2fs\n", ctx->frames, (float)(end_time - start_time) / 1000000);
	FPCLOSE(fp_yuv_in);
	FPCLOSE(fp_out);
    free(buf);
    free(var_buf);

    return RET_OK;
}

int main(int argc, char **argv)
{
	CalcCtx calc_ctx;
	CalcCtx *ctx = &calc_ctx;
	memset(ctx, 0, sizeof(CalcCtx));
    bool help = getarg(false, "-H", "--help", "-?");
    ctx->input = getarg("modify.yuv", "-i", "--input");
    ctx->input_cmp = getarg("origin.yuv", "-o", "--output");
    ctx->output = getarg("psnr.txt", "-p", "--psnr");
    ctx->width = getarg(1920, "-w", "--width");
    ctx->height = getarg(1080, "-h", "--height");
	ctx->frames = getarg(300, "-f", "--frames");	
	ctx->mode = getarg(0, "-m", "--mode");
	ctx->var_ratio_flg = getarg(0, "--var_ratio_flg");

    if (help || argc < 2) {
        cout << "Usage: calculate PSNR" << endl
             << "./video_calc -i=modify.yuv -o=origin.yuv -m=0 "
             << "-w=1920 -h=1080 --frames=300 -p=psnr.txt"
             << endl;

         cout << "Usage: calculate Variance" << endl
             << "./video_calc -i=input.yuv -m=1 --var_ratio_flg=1"
             << "-w=1920 -h=1080 --frames=300 -p=variance.txt"
             << endl;
        return 0;
    }

	if (ctx->mode == CALC_VAR) {
    	calc_var(ctx);
    } else if (ctx->mode == CALC_PSNR) {
		calc_psnr(ctx);
    }

	return 0;
}
