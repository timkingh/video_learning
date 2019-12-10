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
#if 0
int main(int argc, char **argv)
{
    bool help = getarg(false, "-H", "--help", "-?");
    string in_file = getarg("modify.yuv", "-i", "--input");
    string out_file = getarg("origin.yuv", "-o", "--output");
    string psnr_file = getarg("psnr.txt", "-p", "--psnr");
    int w = getarg(1920, "-w", "--width");
    int h = getarg(1080, "-h", "--height");
	int frame_num = getarg(300, "-f", "--frames");;

    if (help || argc < 2) {
        cout << "Usage:" << endl
             << "./calc_psnr -i=modify.yuv -o=origin.yuv "
             << "-w=1920 -h=1080 --frames=300 -p=psnr.txt"
             << endl;
        return 0;
    }
    
    int frame_size = w * h * 3 / 2;
	const char *input_file = in_file.c_str();
	const char *output_file = out_file.c_str();
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
        return 0;
    }

	fp_yuv_org = fopen(output_file, "rb");
    if (fp_yuv_org == NULL) {
        perror("fopen org");
        return 0;
    }

    y_buf = (unsigned char *)malloc(w * h * 2);
    if (y_buf == NULL) {
        printf("malloc y_buf failed\n");
        return 0;
    }

	y_org = (unsigned char *)malloc(w * h * 2);
    if (y_org == NULL) {
        printf("malloc y_org failed\n");
        return 0;
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

	FILE *fp = fopen(psnr_file.c_str(), "ab");
	fprintf(fp, "%s", input_file);
	fprintf(fp, "      %lf\n", psnr);
	fclose(fp);
	printf("%s", input_file);
	printf(" elapsed %.2fs, global psnr = %lf  \n", (float)(end_time - start_time) / 1000000, psnr);
	fclose(fp_yuv_in);
	fclose(fp_yuv_org);
    free(y_buf);
    free(y_org);
}
#endif

static RET calc_var(CalcCtx *ctx)
{   
	uint32_t frame_size = ctx->width * ctx->height;
	const char *input_file = ctx->input.c_str();
	const char *output_file = ctx->output.c_str();
    unsigned char *y_buf;
	int j, i;
	int ret_len_org, ret_len_chg;
	int real_frm_cnt = 0;
	long long ssd;
	long long ssd_global = 0;
	double psnr = 0;
    FILE *fp_yuv_in;
    FILE *fp_out;
    int64_t start_time = time_mdate();
    int64_t end_time;

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

    y_buf = (unsigned char *)malloc(ctx->width * ctx->height * 2);
    if (y_buf == NULL) {
        printf("malloc y_buf failed\n");
        return RET_NOK;
    }

	double sum = 0, sum_square = 0;
	float average = 0, variance = 0;
	real_frm_cnt = 0;
	for (i = 0; i < ctx->frames; i++)
	{
		ret_len_org = fread(y_buf, 1, frame_size, fp_yuv_in);
		if (0 == ret_len_org || ret_len_org < frame_size)
		{
			break;
		}
        printf("fread frame %03d\n", real_frm_cnt);

		real_frm_cnt++;
		ssd = 0;
		for (j = 0; j < frame_size; j++) 
		{
			sum += y_buf[j];
		}
		average = (sum) / frame_size;

		for (j = 0; j < frame_size; j++)
		{
			sum_square += (y_buf[j] - average) * (y_buf[j] - average);
		}
		variance = (sum_square + frame_size / 2) /frame_size;

		ssd_global += ssd;
	}
	fprintf(fp_out, "end of frame %d ave %f var %f\n", real_frm_cnt - 1, average, variance);
    end_time = time_mdate();

	printf("elapsed %.2fs\n", (float)(end_time - start_time) / 1000000);
	FPCLOSE(fp_yuv_in);
	FPCLOSE(fp_out);
    free(y_buf);
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

    if (help || argc < 2) {
        cout << "Usage: calculate PSNR" << endl
             << "./calc_psnr -i=modify.yuv -o=origin.yuv -m=0 "
             << "-w=1920 -h=1080 --frames=300 -p=psnr.txt"
             << endl;

         cout << "Usage: calculate Variance" << endl
             << "./calc_psnr -i=input.yuv -m=1 "
             << "-w=1920 -h=1080 --frames=300 -p=variance.txt"
             << endl;
        return 0;
    }

	if (ctx->mode == CALC_VAR) {
    	calc_var(ctx);
    } else if (ctx->mode == CALC_PSNR) {

    }

	return 0;
}

