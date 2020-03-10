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
    
    FPRINT(fp, "dump quant matrix %003d\n", ctx->rand_seq);
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
        ctx->cqm_4iy[i] = rand() % 255 + 1;
        ctx->seed = (uint32_t)(ctx->cqm_4iy[i] * ctx->seed);
    }

    for (i = 0; i < 64; i++) {
        srand(ctx->seed);
        ctx->cqm_8iy[i] = rand() % 255 + 1;
        ctx->seed = (uint32_t)(ctx->cqm_8iy[i] * ctx->seed);
    }
}

static RET quant_matrix_research(CalcCtx *ctx, FILE *fp)
{
	int mf, k, i, mf_float;
	int qp = 1;
	uint64_t scale_fixed_point;
	uint64_t mf_left_shift = (1 << ctx->mf_fixed_point_bits);
	uint8_t right_shift_bits = ctx->mf_fixed_point_bits;
    uint16_t quant4_mf_lu[16];
    uint16_t quant8_mf_lu[64];
    uint32_t diff_cnt = 0;

	for (qp = 1; qp < 52; qp++) {
		for (i = 0; i < 16; i++) {
			k = (i & 1) + ((i >> 2) & 1);

			scale_fixed_point = H264E_DIV(mf_left_shift, ctx->cqm_4iy[i]);
			mf = (int32_t)((h264e_quant4_scale[qp % 6][k] * 16 * scale_fixed_point +
					(uint64_t)(1 << (right_shift_bits - 1))) >> right_shift_bits);

            mf_float = H264E_DIV((h264e_quant4_scale[qp % 6][k] * 16), ctx->cqm_4iy[i]);
            if (mf != mf_float) {
                diff_cnt++;
                if (abs(mf - mf_float) >= (int)ctx->mf_diff_thresh) {
                    FPRINT(fp, "quant4x4 qp %d i %d mf %d %d\n", qp, i, mf, mf_float);
                }
            }

			/* Note: quant4_mf_lu's type is uint16_t, which is not enough to store
			 *		 mf when q is zero(because quant4_mf_lu(i = 0) is 69898). However,
			 *		 q=0 will not be used in reality, ignore it.(2018.07.12)
			 */
			quant4_mf_lu[i]  = mf = H264E_SHIFT(mf, qp / 6 - 1);
		}
	}

    FPRINT(fp, "rand %d quant4x4 diff %d/%d\n", ctx->rand_seq, diff_cnt, 52*16);  

    diff_cnt = 0;
	for (qp = 1; qp < 52; qp++) {
		for (i = 0; i < 64; i++) {
			k = h264e_quant8_scan[((i >> 1) & 12) | (i & 3)];

			scale_fixed_point = H264E_DIV(mf_left_shift, ctx->cqm_8iy[i]);
			mf = (int32_t)((h264e_quant8_scale[qp % 6][k] * 16 * scale_fixed_point +
				   (uint64_t)(1 << (right_shift_bits - 1))) >> right_shift_bits);

            mf_float = H264E_DIV((h264e_quant8_scale[qp % 6][k] * 16), ctx->cqm_8iy[i]);
            if (mf != mf_float) {
                diff_cnt++;                
                if (abs(mf - mf_float) >= (int)ctx->mf_diff_thresh) {
                    FPRINT(fp, "quant8x8 qp %d i %d mf %d %d\n", qp, i, mf, mf_float);
                }
            }
                   
			quant8_mf_lu[i] = mf = H264E_SHIFT(mf, qp / 6);
		}
	}
    FPRINT(fp, "rand %d quant8x8 diff %d/%d\n", ctx->rand_seq, diff_cnt, 52*64);  

    return RET_OK;
}

RET calc_quant_matrix(CalcCtx *ctx)
{
    uint32_t idx = 0;
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

    FPRINT(fp, "mf_fixed_point_bits %d\n", ctx->mf_fixed_point_bits);    
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
    
   	end_time = time_mdate();

	printf("rand %d elapsed %.2fs\n", ctx->rand_cnt, (float)(end_time - start_time) / 1000000); 
    FPCLOSE(fp);
	return RET_OK;
}
