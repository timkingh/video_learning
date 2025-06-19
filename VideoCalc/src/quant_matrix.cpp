#define _CRT_SECURE_NO_WARNINGS
#include "quant_matrix.h"
#include <time.h>

#define H264E_DIV(n,d) (((n) + ((d)>>1)) / (d))
#define H264E_SHIFT(x,s) ((s)<=0 ? (x)<<-(s) : ((x)+(1<<((s)-1)))>>(s))

/* default quant matrices */
static const uint8_t x264_cqm_jvt4i[16] =
{
    6, 13, 20, 28,
    13, 20, 28, 32,
    20, 28, 32, 37,
    28, 32, 37, 42
};

static const uint8_t x264_cqm_jvt4p[16] =
{
    10, 14, 20, 24,
    14, 20, 24, 27,
    20, 24, 27, 30,
    24, 27, 30, 34
};

static const uint8_t x264_cqm_jvt8i[64] =
{
    6, 10, 13, 16, 18, 23, 25, 27,
    10, 11, 16, 18, 23, 25, 27, 29,
    13, 16, 18, 23, 25, 27, 29, 31,
    16, 18, 23, 25, 27, 29, 31, 33,
    18, 23, 25, 27, 29, 31, 33, 36,
    23, 25, 27, 29, 31, 33, 36, 38,
    25, 27, 29, 31, 33, 36, 38, 40,
    27, 29, 31, 33, 36, 38, 40, 42
};

static const uint8_t x264_cqm_jvt8p[64] =
{
    9, 13, 15, 17, 19, 21, 22, 24,
    13, 13, 17, 19, 21, 22, 24, 25,
    15, 17, 19, 21, 22, 24, 25, 27,
    17, 19, 21, 22, 24, 25, 27, 28,
    19, 21, 22, 24, 25, 27, 28, 30,
    21, 22, 24, 25, 27, 28, 30, 32,
    22, 24, 25, 27, 28, 30, 32, 33,
    24, 25, 27, 28, 30, 32, 33, 35
};

uint16_t h264e_quant4_scale[6][3] =
{
    { 13107, 8066, 5243 },
    { 11916, 7490, 4660 },
    { 10082, 6554, 4194 },
    {  9362, 5825, 3647 },
    {  8192, 5243, 3355 },
    {  7282, 4559, 2893 },
};

uint8_t h264e_quant8_scan[16] =
{
    0, 3, 4, 3, 3, 1, 5, 1, 4, 5, 2, 5, 3, 1, 5, 1
};

uint16_t h264e_quant8_scale[6][6] =
{
    { 13107, 11428, 20972, 12222, 16777, 15481 },
    { 11916, 10826, 19174, 11058, 14980, 14290 },
    { 10082,  8943, 15978,  9675, 12710, 11985 },
    {  9362,  8228, 14913,  8931, 11984, 11259 },
    {  8192,  7346, 13159,  7740, 10486,  9777 },
    {  7282,  6428, 11570,  6830,  9118,  8640 }
};

static void dump_scaling_list(CalcCtx *ctx, FILE *fp)
{
    uint32_t i;

    FPRINT(fp, "dump quant matrix %03d\n", ctx->rand_seq);
    FPRINT(fp, "matrix 4x4:\n");
    for (i = 0; i < 16; i++) {
        FPRINT(fp, "%3d ", ctx->cqm_4iy[i]);

        if ((i + 1) % 4 == 0) {
            FPRINT(fp, "\n");
        }
    }

    FPRINT(fp, "matrix 8x8:\n");
    for (i = 0; i < 64; i++) {
        FPRINT(fp, "%3d ", ctx->cqm_8iy[i]);

        if ((i + 1) % 8 == 0) {
            FPRINT(fp, "\n");
        }
    }
    FPRINT(fp, "\n\n");
}

static void copy_default_matrix(CalcCtx *ctx)
{
    if (ctx->rand_seq == 0) {
        memcpy(ctx->cqm_4iy, x264_cqm_jvt4i, sizeof(uint8_t) * 16);
        memcpy(ctx->cqm_8iy, x264_cqm_jvt8i, sizeof(uint8_t) * 64);
    } else {
        memcpy(ctx->cqm_4iy, x264_cqm_jvt4p, sizeof(uint8_t) * 16);
        memcpy(ctx->cqm_8iy, x264_cqm_jvt8p, sizeof(uint8_t) * 64);
    }
}

static void generate_scaling_list(CalcCtx *ctx)
{
    uint32_t i;

    for (i = 0; i < 16; i++) {
        srand(ctx->seed);
        ctx->cqm_4iy[i] = ctx->rand_seq + 1;//rand() % 255 + 1;
        ctx->seed = (uint32_t)(ctx->cqm_4iy[i] * ctx->seed);
    }

    for (i = 0; i < 64; i++) {
        srand(ctx->seed);
        ctx->cqm_8iy[i] = ctx->rand_seq + 1;//rand() % 255 + 1;
        ctx->seed = (uint32_t)(ctx->cqm_8iy[i] * ctx->seed);
    }
}

static RET quant_matrix_research(CalcCtx *ctx, FILE *fp)
{
	int mf, k, i, mf_float;
	int qp = 1;
	uint64_t scale_fixed_point, bias_fixed_point;
	uint64_t mf_left_shift = ((uint64_t)1 << ctx->real_fixed_bits);
	uint8_t right_shift_bits = ctx->real_fixed_bits;
    uint64_t bias_left_shift = ((uint64_t)1 << ctx->real_bias_fixed_bits);
    uint8_t bias_precision_bits = 5;
    uint16_t quant4_mf_lu[16];
    uint16_t quant8_mf_lu[64];
    uint16_t quant4_bias_lu[16];
    uint16_t quant8_bias_lu[64];
    uint16_t bias_float;
    uint32_t diff_cnt = 0;
    static int max_mf = 0;
    static int max_quant4_mf = 0;
    static int max_quant4_bias = 0;

	for (qp = 1; qp < 52; qp++) {
		for (i = 0; i < 16; i++) {
			k = (i & 1) + ((i >> 2) & 1);

			scale_fixed_point = H264E_DIV(mf_left_shift, ctx->cqm_4iy[i]);
			mf = (int32_t)((h264e_quant4_scale[qp % 6][k] * 16 * scale_fixed_point +
					((uint64_t)1 << (right_shift_bits - 1))) >> right_shift_bits);

            mf_float = H264E_DIV((h264e_quant4_scale[qp % 6][k] * 16), ctx->cqm_4iy[i]);
            if (mf != mf_float) {
                ctx->sum_diff += abs(mf - mf_float);
                diff_cnt++;
                if (abs(mf - mf_float) >= (int)ctx->mf_diff_thresh) {
                    FPRINT(fp, "quant4x4 qp %d i %d mf %d %d\n", qp, i, mf, mf_float);
                }
            }

            if (mf > max_mf) {
                max_mf = mf;
                FPRINT(fp, "max_mf %d qp %d i %d scale %d\n", max_mf, qp, i, ctx->cqm_4iy[i]);
            }

			/* Note: quant4_mf_lu's type is uint16_t, which is not enough to store
			 *		 mf when q is zero(because quant4_mf_lu(i = 0) is 69898). However,
			 *		 q=0 will not be used in reality, ignore it.(2018.07.12)
			 */
			quant4_mf_lu[i]  = mf = H264E_SHIFT(mf, qp / 6 - 1);
            if (quant4_mf_lu[i] > max_quant4_mf) {
                max_quant4_mf = quant4_mf_lu[i];
                FPRINT(fp, "max_quant_mf %d qp %d i %d scale %d\n", max_quant4_mf, qp, i, ctx->cqm_4iy[i]);
            }

            bias_fixed_point = H264E_DIV(bias_left_shift, mf);
            quant4_bias_lu[i] = (uint16_t)(((ctx->bias << bias_precision_bits) *
                                           bias_fixed_point + bias_left_shift / 2)
                                           >> ctx->real_bias_fixed_bits);
            bias_float = H264E_DIV(ctx->bias << bias_precision_bits, mf);
            if (quant4_bias_lu[i] != bias_float) {
                ctx->sum_diff_bias += abs(quant4_bias_lu[i] - bias_float);
            }

            if (quant4_bias_lu[i] > max_quant4_bias) {
                max_quant4_bias = quant4_bias_lu[i];
                FPRINT(fp, "max_quant_bias %d qp %d i %d scale %d mf %d\n", max_quant4_bias, qp, i, ctx->cqm_4iy[i], mf);
            }
		}
	}

    if (diff_cnt) {
        //FPRINT(fp, "rand %d quant4x4 diff %d/%d\n", ctx->rand_seq, diff_cnt, 52*16);
    }

    diff_cnt = 0;
	for (qp = 1; qp < 52; qp++) {
		for (i = 0; i < 64; i++) {
			k = h264e_quant8_scan[((i >> 1) & 12) | (i & 3)];

			scale_fixed_point = H264E_DIV(mf_left_shift, ctx->cqm_8iy[i]);
			mf = (int32_t)((h264e_quant8_scale[qp % 6][k] * 16 * scale_fixed_point +
				   ((uint64_t)1 << (right_shift_bits - 1))) >> right_shift_bits);

            mf_float = H264E_DIV((h264e_quant8_scale[qp % 6][k] * 16), ctx->cqm_8iy[i]);
            if (mf != mf_float) {
                ctx->sum_diff += abs(mf - mf_float);
                diff_cnt++;
                if (abs(mf - mf_float) >= (int)ctx->mf_diff_thresh) {
                    FPRINT(fp, "quant8x8 qp %d i %d mf %d %d\n", qp, i, mf, mf_float);
                }
            }

            if (mf > max_mf) {
                max_mf = mf;
                FPRINT(fp, "max_mf_8 %d qp %d i %d scale %d\n", max_mf, qp, i, ctx->cqm_8iy[i]);
            }

			quant8_mf_lu[i] = mf = H264E_SHIFT(mf, qp / 6);
            if (quant8_mf_lu[i] > max_quant4_mf) {
                max_quant4_mf = quant8_mf_lu[i];
                FPRINT(fp, "max_quant8_mf %d qp %d i %d scale %d\n", max_quant4_mf, qp, i, ctx->cqm_8iy[i]);
            }

            bias_fixed_point = H264E_DIV(bias_left_shift, mf);
            //bias_fixed_point = bias_left_shift / mf + 1;
            quant8_bias_lu[i] = (uint16_t)(((ctx->bias << bias_precision_bits) *
                                           bias_fixed_point + bias_left_shift / 2)
                                           >> ctx->real_bias_fixed_bits);
            bias_float = H264E_DIV(ctx->bias << bias_precision_bits, mf);
            if (quant8_bias_lu[i] != bias_float) {
                ctx->sum_diff_bias += abs(quant8_bias_lu[i] - bias_float);
            }

            if (quant8_bias_lu[i] > max_quant4_bias) {
                max_quant4_bias = quant8_bias_lu[i];
                FPRINT(fp, "max_quant8_bias %d qp %d i %d scale %d\n mf %d", max_quant4_bias, qp, i, ctx->cqm_8iy[i], mf);
            }
		}
	}

    if (diff_cnt) {
        //FPRINT(fp, "rand %d quant8x8 diff %d/%d\n", ctx->rand_seq, diff_cnt, 52*64);
    }

    ctx->max_mf = max_mf;
    ctx->max_quant_mf = max_quant4_mf;
    ctx->max_quant_bias = max_quant4_bias;

    return RET_OK;
}

RET calc_quant_matrix(CalcCtx *ctx)
{
    uint32_t idx = 0, bits = 22;
	int64_t start_time = time_mdate();
	int64_t end_time;
	const char *output_file = ctx->output.c_str();
    FILE *fp = fopen(output_file, "a+");
    if (fp == NULL) {
        perror("fopen output file\n");
    }

    if (ctx->default_matrix) {
        ctx->rand_cnt = 2;
        FPRINT(fp, "default matrix result:\n");
    }

    if (ctx->mf_fixed_point_bits < 22 || ctx->mf_fixed_point_bits > 45) {
        printf("mf_fixed_point_bits %d error, should be in range [22,45]\n", ctx->mf_fixed_point_bits);
        return RET_NOK;
    }

    for (bits = 25; bits <= ctx->bias_fixed_point_bits; bits++) {
        ctx->real_fixed_bits = 28;
        ctx->real_bias_fixed_bits = bits;
        ctx->sum_diff = 0;
        ctx->sum_diff_bias = 0;
        FPRINT(fp, "mf_fixed_point_bits %d\n", ctx->real_fixed_bits);
        ctx->seed = (uint32_t)time(NULL);

        for (idx = 0; idx < ctx->rand_cnt; idx++) {
            if (idx % ctx->log_frames == (ctx->log_frames - 1)) {
                printf("rand seq %d\n", idx);
            }

            ctx->rand_seq = idx;
            if (ctx->default_matrix) {
                copy_default_matrix(ctx);
            } else {
                generate_scaling_list(ctx);
            }

            if (ctx->dump_matrix) {
                dump_scaling_list(ctx, fp);
            }

            quant_matrix_research(ctx, fp);
        }

        //FPRINT(fp, "mf_fixed_point_bits %d rand %d sum_diff %lld\n", ctx->real_fixed_bits, ctx->rand_cnt, ctx->sum_diff);
        //FPRINT(fp, "bias %d bias_fixed_point_bits %d sum_diff_bias %lld\n", ctx->bias,
         //         ctx->real_bias_fixed_bits, ctx->sum_diff_bias);
        FPRINT(fp, "max_mf %d max_quant_mf %d max_quant_bias %d\n", ctx->max_mf, ctx->max_quant_mf, ctx->max_quant_bias);
    }

   	end_time = time_mdate();

	printf("rand %d elapsed %.2fs\n", ctx->rand_cnt, (float)(end_time - start_time) / 1000000);
    FPCLOSE(fp);
	return RET_OK;
}
