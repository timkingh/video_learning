#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define WIDTH_MAX (3840)
#define HEIGHT_MAX (2160)
#define PIXEL_MAX 255

unsigned char y_buf[WIDTH_MAX * HEIGHT_MAX*2];
unsigned char u_buf[WIDTH_MAX * HEIGHT_MAX / 4];
unsigned char v_buf[WIDTH_MAX * HEIGHT_MAX / 4];

unsigned char y_org[WIDTH_MAX * HEIGHT_MAX*2];
unsigned char u_org[WIDTH_MAX * HEIGHT_MAX / 4];
unsigned char v_org[WIDTH_MAX * HEIGHT_MAX / 4];

static double x264_psnr(double sqe, double size)
{
	double mse = sqe / (PIXEL_MAX * PIXEL_MAX * size);
	if (mse <= 0.0000000001)  /* Max 100dB */
		return 100;

	return -10.0 * log10(mse);
}

int main(int argc, char **argv)
{
    int w = 960;
    int h = 448;
    int frame_num = 300;
    int frame_size = w*h * 3 / 2;
    int y_size = w*h;
    int u_size = w*h/4;
    int v_size = u_size;
	const char *input_file = NULL;
	const char *output_file = NULL;
	int j, i;
	int y_stride = w;
	int uv_stride = w / 2;
	int ret_len_org, ret_len_chg;
	int real_frm_cnt = 0;
	long long ssd;
	long long ssd_global = 0;
	double psnr = 0;
    FILE *fp_yuv_in;
    FILE *fp_yuv_org;

	for (i = 1; i < argc; ++i){
		if (strcmp(argv[i], "-i") == 0){
			input_file = argv[i + 1];
			i++;
			continue;
		}

		if (strcmp(argv[i], "-o") == 0){
			output_file = argv[i + 1];
			i++;
			continue;
		}

		if (strcmp(argv[i], "-w") == 0) {
			char *endptr;
			w = strtol(argv[i + 1], &endptr, 10);
			if (*endptr)
			{
				printf("Cannot parse width value: %s\n", argv[i + 1]);
			}
			i++;
			continue;
		}

		if (strcmp(argv[i], "-h") == 0) {
			char *endptr;
			h = strtol(argv[i + 1], &endptr, 10);
			if (*endptr)
			{
				printf("Cannot parse height value: %s\n", argv[i + 1]);
			}
			i++;
			continue;
		}

		if (strcmp(argv[i], "-n") == 0) {
			char *endptr;
			frame_num = strtol(argv[i + 1], &endptr, 10);
			if (*endptr)
			{
				printf("Cannot parse frame number value: %s\n", argv[i + 1]);
			}
			i++;
			continue;
		}
	}

	y_stride = w;
	uv_stride = w / 2;
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

	real_frm_cnt = 0;
	frame_size = w*h * 3 / 2;
	for (i = 0; i < frame_num; i++)
	{
		ret_len_org = fread(y_buf, 1, frame_size, fp_yuv_in);
		ret_len_chg = fread(y_org, 1, frame_size, fp_yuv_org);
		if (0 == ret_len_chg || 0 == ret_len_org || ret_len_chg < frame_size || ret_len_org < frame_size)
		{
			break;
		}

		real_frm_cnt++;
		ssd = 0;
		for (j = 0; j < frame_size; j++)
			ssd += (y_buf[j] - y_org[j])*(y_buf[j] - y_org[j]);

		ssd_global += ssd;
	}
	printf("end of frame %d\n", real_frm_cnt);

	psnr = x264_psnr((double)ssd_global / real_frm_cnt, frame_size);
	FILE *fp = fopen("psnr.txt", "ab");
	fprintf(fp, input_file);
	fprintf(fp, "      %lf\n", psnr);
	fclose(fp);
	printf(input_file);
	printf(" global psnr = %lf  \n", psnr);
	fclose(fp_yuv_in);
	fclose(fp_yuv_org);
}


