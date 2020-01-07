#define _CRT_SECURE_NO_WARNINGS
#include "video_common.h"
#include "getopt.hpp"
#include "calc_var.h"
#include "calc_psnr.h"
#include "calc_histogram.h"
#include "weight_yuv.h"

using namespace std;

enum MODE {
	CALC_PSNR = 0,
	CALC_VAR = 1,
	CALC_HIST = 2, /* Histogram */
	WEIGHT_YUV = 3,
};

static void show_help()
{       
	cout << "Usage: calculate PSNR" << endl
         << "VidoeCalc -i=modify.yuv -o=origin.yuv -m=0 "
         << "-w=1920 -h=1080 --frames=300 -p=psnr.txt --log_frames=10"
         << endl;

    cout << "Usage: calculate Variance" << endl
         << "VideoCalc -i=input.yuv -m=1 --var_ratio_flg=1 "
         << "-w=1920 -h=1080 --frames=300 -p=variance.txt"
         << endl;

	cout << "Usage: calculate histogram" << endl
         << "VidoeCalc -i=modify.yuv -m=2 "
         << "-w=1920 -h=1080 --frames=300 -p=hist.txt"
         << endl;

    cout << "Usage: calculate weighted yuv" << endl
         << "VideoCalc -i=input.yuv -o=weighted.yuv -m=3 "
         << "-w=1920 -h=1080 --frames=300 -p=param.txt --combo_yuv=combo.yuv "
         << endl;	 
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
    ctx->out_yuv = getarg("combo.yuv", "-q", "--combo_yuv");
    ctx->width = getarg(1920, "-w", "--width");
    ctx->height = getarg(1080, "-h", "--height");
	ctx->frames = getarg(300, "-f", "--frames");	
	ctx->mode = getarg(0, "-m", "--mode");
	ctx->var_ratio_flg = getarg(0, "--var_ratio_flg");
	ctx->log_frames = getarg(1, "--log_frames");

    if (help || argc < 2) {
		show_help();
        return 0;
    }

	/* Functions' order must match with enum MODE */
	func demo[] = {
		calc_psnr,
		calc_var,
		calc_histogram,
		weight_yuv
	};

	demo[ctx->mode](ctx);

	return 0;
}
