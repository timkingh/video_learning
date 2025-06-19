#include "transform_yuv_format.h"

typedef struct {
    CalcCtx *calc_ctx;
    uint8_t *src_buf;
    uint8_t *dst_buf;
    FILE *fp_in;
    FILE *fp_out;
} DrawNumCtx;


// Simple ASCII art for the number '5'
const char *number5[] = {
    "  ##   ",
    " #  #  ",
    "      ",
    " #  #  ",
    "  ##   "
};

static void draw_number(uint8_t *yuv, int width, int height,
                            const char **number, int x, int y)
{
    int num_height = 5;//sizeof(**number) / sizeof(char*);
    int num_width = strlen(number[0]);
    int i, j;

    for (i = 0; i < 32; i++) {
        for (j = 0; j < 32; j++) {
            int pos = (y + i) * width + x + j;
            if(i == 16 && j >= 8 && j <= 15) {
                yuv[pos] = 76; // 中间一列
            } else if(i == 16 && j >= 0 && j <= 7) {
                yuv[pos] = 76; // 最左边的一列
            } else if(i == 16 && j >= 16 && j <= 23) {
                yuv[pos] = 76; // 最右边的一列
            } else {
                // yuv[pos] = 0; // 其他位置
            }

            // if (i == j && j % 4 == 0) {
            //     int pos = (y + i) * width + x + j;
            //     if (pos < width * height) {
            //         yuv[pos] = 76; // Set the Y value for red
            //     }
            // }
        }
    }
}

static RET draw_number_on_yuv_init(CalcCtx *calc_ctx, DrawNumCtx *draw_ctx)
{
    const char *input_file = calc_ctx->input.c_str();
    const char *output_file = calc_ctx->input_cmp.c_str();

    draw_ctx->fp_in = fopen(input_file, "rb");
    if (draw_ctx->fp_in == NULL) {
        perror("fopen input");
        return RET_NOK;
    }

    draw_ctx->fp_out = fopen(output_file, "w+");
    if (draw_ctx->fp_out == NULL) {
        perror("fopen output");
        return RET_NOK;
    }

    draw_ctx->calc_ctx = calc_ctx;

    draw_ctx->src_buf = (uint8_t*)malloc(calc_ctx->width * calc_ctx->height * 3 / 2);
    if (draw_ctx->src_buf == NULL) {
        perror("malloc src_buf");
        return RET_NOK;
    }

    draw_ctx->dst_buf = (uint8_t*)malloc(calc_ctx->width * calc_ctx->height * 3 / 2);
    if (draw_ctx->dst_buf == NULL) {
        perror("malloc dst_buf");
        return RET_NOK;
    }

    return RET_OK;
}

static void draw_number_on_yuv_deinit(DrawNumCtx *draw_ctx)
{
    FPCLOSE(draw_ctx->fp_in);
    FPCLOSE(draw_ctx->fp_out);
    FREE(draw_ctx->src_buf);
    FREE(draw_ctx->dst_buf);
}

static RET concat_yuv_files(DrawNumCtx *draw_ctx)
{
    int frames = draw_ctx->calc_ctx->frames;
    int width = draw_ctx->calc_ctx->width;
    int height = draw_ctx->calc_ctx->height;
    int k;
    char filename[100];
    FILE *fp_in;

    for (k = 0; k < frames; k++) {
        printf("Processing frame %d\n", k);
        sprintf(filename, "/mnt/winshare/out/output_1080p_%d.yuv", k);
        fp_in = fopen(filename, "rb");
        if (fp_in == NULL) {
            perror("fopen");
            return RET_NOK;
        }

        if (fread(draw_ctx->src_buf, 1, width * height * 3 / 2, fp_in) != width * height * 3 / 2) {
            perror("fread");
            return RET_NOK;
        }

        if (fwrite(draw_ctx->src_buf, 1, width * height * 3 / 2, draw_ctx->fp_out) != width * height * 3 / 2) {
            perror("fwrite");
            return RET_NOK;
        }

        fclose(fp_in);
    }

	return RET_OK;
}

static RET draw_number_on_yuv_process(DrawNumCtx *draw_ctx)
{
    uint8_t *buffer = draw_ctx->src_buf;
    int width = draw_ctx->calc_ctx->width;
    int height = draw_ctx->calc_ctx->height;

    // Read the YUV data from the input file
    // if (fread(buffer, 1, width * height * 3 / 2, draw_ctx->fp_in) != width * height * 3 / 2) {
    //     perror("fread");
    //     return RET_NOK;
    // }

    concat_yuv_files(draw_ctx);

    // Draw the number '5' at a specific position
    // draw_number(buffer, width, height, number5, 100, 100);

    // Write the modified YUV data to the output file
    // if (fwrite(buffer, 1, width * height * 3 / 2, draw_ctx->fp_out) != width * height * 3 / 2) {
    //     perror("fwrite");
    //     return RET_NOK;
    // }

    return RET_OK;
}

RET transform_yuv_format(CalcCtx *ctx)
{
    DrawNumCtx draw_ctx;
    memset(&draw_ctx, 0, sizeof(DrawNumCtx));

    draw_number_on_yuv_init(ctx, &draw_ctx);

    draw_number_on_yuv_process(&draw_ctx);

    draw_number_on_yuv_deinit(&draw_ctx);

    return RET_OK;
}