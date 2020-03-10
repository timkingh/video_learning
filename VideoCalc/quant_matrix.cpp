#define _CRT_SECURE_NO_WARNINGS
#include "quant_matrix.h"
#include <time.h>

#define QUANT_RESIDUAL_PRECISE (0)
#define MF_FIXED_POINT_BITS    (22)
#define H264E_DIV(n,d) (((n) + ((d)>>1)) / (d))
#define H264E_SHIFT(x,s) ((s)<=0 ? (x)<<-(s) : ((x)+(1<<((s)-1)))>>(s))


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

static void generate_scaling_list(CalcCtx *ctx)
{
    uint32_t i;
    
    for (i = 0; i < 16; i++) {
        srand(ctx->seed);
        ctx->cqm_4iy[i] = rand() % 255 + 1;
        ctx->seed = (uint32_t)(ctx->cqm_4iy[i] * (i + 1));
    }

    for (i = 0; i < 64; i++) {
        srand(ctx->seed);
        ctx->cqm_8iy[i] = rand() % 255 + 1;
        ctx->seed = (uint32_t)(ctx->cqm_8iy[i] * (i + 1));
    }
}

static RET quant_matrix_research(CalcCtx *ctx)
{
	int q, mf, k, i, i_list;
	int qp_lu = 1;
	uint64_t scale_fixed_point;
	uint64_t mf_left_shift = (1 << MF_FIXED_POINT_BITS);
	uint8_t right_shift_bits = MF_FIXED_POINT_BITS - QUANT_RESIDUAL_PRECISE;
	uint8_t bias_precision_bits = (5 + QUANT_RESIDUAL_PRECISE);
    uint16_t quant4_mf_lu[2][16];
    uint16_t quant8_mf_lu[2][64];

	q = qp_lu;
	for (i_list = 0; i_list < 2; i_list++) {
		for (i = 0; i < 16; i++) {
			k = (i & 1) + ((i >> 2) & 1);

			scale_fixed_point = H264E_DIV(mf_left_shift, ctx->cqm_4iy[i]);
			mf = (int32_t)((h264e_quant4_scale[q % 6][k] * 16 * scale_fixed_point +
					(uint64_t)(1 << (right_shift_bits - 1))) >> right_shift_bits);

			/* Note: quant4_mf_lu's type is uint16_t, which is not enough to store
			 *		 mf when q is zero(because quant4_mf_lu(i = 0) is 69898). However,
			 *		 q=0 will not be used in reality, ignore it.(2018.07.12)
			 */
			quant4_mf_lu[i_list][i]  = mf = H264E_SHIFT(mf, q / 6 - 1);
		}
	}

	q = qp_lu;
	for (i_list = 0; i_list < 2; i_list++) {
		for (i = 0; i < 64; i++) {
			k = h264e_quant8_scan[((i >> 1) & 12) | (i & 3)];

			scale_fixed_point = H264E_DIV(mf_left_shift, ctx->cqm_8iy[i]);
			mf = (int32_t)((h264e_quant8_scale[q % 6][k] * 16 * scale_fixed_point +
				   (uint64_t)(1 << (right_shift_bits - 1))) >> right_shift_bits);
			quant8_mf_lu[i_list][i] = mf = H264E_SHIFT(mf, q / 6);
		}
	}

    return RET_OK;
}

RET calc_quant_matrix(CalcCtx *ctx)
{
    uint32_t idx = 0;
	const char *output_file = ctx->output.c_str();
    FILE *fp = fopen(output_file, "a+");
    if (fp == NULL) {
        perror("fopen output file\n");
    }
    
    ctx->seed = (uint32_t)time(NULL);

    for (idx = 0; idx < ctx->rand_cnt; idx++) {
        ctx->rand_seq = idx;
        generate_scaling_list(ctx);

        dump_scaling_list(ctx, fp);
        
        quant_matrix_research(ctx);
    }
    
    FPCLOSE(fp);
	return RET_OK;
}
