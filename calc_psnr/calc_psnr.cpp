#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#if _WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif
#include "getopt.hpp"

#define PIXEL_MAX 255

#if _WIN32
typedef long long int64_t;
#endif

using namespace std;

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
