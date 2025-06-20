#include "transform_yuv_format.h"

typedef struct {
    CalcCtx *calc_ctx;
    uint8_t *src_buf;
    uint8_t *dst_buf;
    FILE *fp_in;
    FILE *fp_out;
} TransYuvCtx;

static RET trans_yuv_fmt_init(CalcCtx *calc_ctx, TransYuvCtx *trans_ctx)
{
    const char *input_file = calc_ctx->input.c_str();
    const char *output_file = calc_ctx->input_cmp.c_str();

    trans_ctx->fp_in = fopen(input_file, "rb");
    if (trans_ctx->fp_in == NULL) {
        perror("fopen input");
        return RET_NOK;
    }

    trans_ctx->fp_out = fopen(output_file, "w+");
    if (trans_ctx->fp_out == NULL) {
        perror("fopen output");
        return RET_NOK;
    }

    trans_ctx->calc_ctx = calc_ctx;

    trans_ctx->src_buf = (uint8_t*)malloc(calc_ctx->width * calc_ctx->height * 3);
    if (trans_ctx->src_buf == NULL) {
        perror("malloc src_buf");
        return RET_NOK;
    }

    trans_ctx->dst_buf = (uint8_t*)malloc(calc_ctx->width * calc_ctx->height * 3);
    if (trans_ctx->dst_buf == NULL) {
        perror("malloc dst_buf");
        return RET_NOK;
    }

    return RET_OK;
}

static void trans_yuv_fmt_deinit(TransYuvCtx *trans_ctx)
{
    FPCLOSE(trans_ctx->fp_in);
    FPCLOSE(trans_ctx->fp_out);
    FREE(trans_ctx->src_buf);
    FREE(trans_ctx->dst_buf);
}

static RET trans_yuv420sp_to_yuv420p(TransYuvCtx *trans_ctx)
{
    RET ret = RET_OK;
    uint8_t *yuv420sp = trans_ctx->src_buf;
    uint8_t *yuv420p = trans_ctx->dst_buf;
    int width = trans_ctx->calc_ctx->width;
    int height = trans_ctx->calc_ctx->height;
    size_t frm_len = width * height * 3 / 2;
    int y_size = width * height;
    int uv_size = y_size / 4;

    // Read the YUV data from the input file
    if (fread(yuv420sp, 1, width * height * 3 / 2, trans_ctx->fp_in) != frm_len) {
        perror("fread");
        return RET_NOK;
    }

    // 复制Y分量
    memcpy(yuv420p, yuv420sp, y_size);

    // 处理UV分量（NV12/NV21转YUV420P）
    uint8_t *uv_sp = yuv420sp + y_size;  // YUV420SP的UV起始位置
    uint8_t *u_p = yuv420p + y_size;     // YUV420P的U起始位置
    uint8_t *v_p = u_p + uv_size;        // YUV420P的V起始位置

    // 分离交错的UV分量
    for (int i = 0; i < uv_size; i++) {
        u_p[i] = uv_sp[2 * i];      // 对于NV12，U在前；对于NV21，V在前
        v_p[i] = uv_sp[2 * i + 1];  // 对于NV12，V在后；对于NV21，U在后
    }

    // Write the modified YUV data to the output file
    if (fwrite(yuv420p, 1, width * height * 3 / 2, trans_ctx->fp_out) != frm_len) {
        perror("fwrite");
        return RET_NOK;
    }

    return ret;
}

static RET trans_yuv420p_to_yuv420sp(TransYuvCtx *trans_ctx)
{
    RET ret = RET_OK;
    uint8_t *yuv420p = trans_ctx->src_buf;
    uint8_t *yuv420sp = trans_ctx->dst_buf;
    int width = trans_ctx->calc_ctx->width;
    int height = trans_ctx->calc_ctx->height;
    size_t frm_len = width * height * 3 / 2;
    int y_size = width * height;
    int uv_size = y_size / 4;

    // Read the YUV data from the input file
    if (fread(yuv420p, 1, width * height * 3 / 2, trans_ctx->fp_in) != frm_len) {
        perror("fread");
        return RET_NOK;
    }

    // 复制Y分量
    memcpy(yuv420sp, yuv420p, y_size);

    // 获取YUV420P中的U和V分量指针
    unsigned char* u_p = yuv420p + y_size;
    unsigned char* v_p = u_p + uv_size;

    // YUV420SP的UV分量起始位置（NV12格式）
    unsigned char* uv_sp = yuv420sp + y_size;

    // 合并U和V分量到交错排列的UV平面
    for (int i = 0; i < uv_size; i++) {
        uv_sp[2 * i] = u_p[i];    // U分量
        uv_sp[2 * i + 1] = v_p[i]; // V分量
    }

    // Write the modified YUV data to the output file
    if (fwrite(yuv420sp, 1, width * height * 3 / 2, trans_ctx->fp_out) != frm_len) {
        perror("fwrite");
        return RET_NOK;
    }

    return ret;
}

static RET trans_yuv422p_to_yuv420p(TransYuvCtx *trans_ctx)
{
    RET ret = RET_OK;
    uint8_t *yuv422p = trans_ctx->src_buf;
    uint8_t *yuv420p = trans_ctx->dst_buf;
    int width = trans_ctx->calc_ctx->width;
    int height = trans_ctx->calc_ctx->height;
    size_t frm_len_420 = width * height * 3 / 2;
    size_t frm_len_422 = width * height * 2;
    int y_size = width * height;
    int uv_size_422 = y_size / 2;  // YUV422P的UV总大小
    int uv_size_420 = y_size / 4;  // YUV420P的UV总大小

    // Read the YUV data from the input file
    if (fread(yuv422p, 1, frm_len_422, trans_ctx->fp_in) != frm_len_422) {
        perror("fread");
        return RET_NOK;
    }

    // 复制Y分量（直接1:1拷贝）
    memcpy(yuv420p, yuv422p, y_size);

    // 获取YUV422P的U和V分量指针
    unsigned char* u_422 = yuv422p + y_size;
    unsigned char* v_422 = u_422 + uv_size_422;

    // 获取YUV420P的U和V分量指针
    unsigned char* u_420 = yuv420p + y_size;
    unsigned char* v_420 = u_420 + uv_size_420;

    // 色度下采样：每相邻2x2的UV像素取平均值
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            // 计算当前块在422和420中的位置
            int pos_422 = (y * width + x) / 2;
            int pos_420 = (y / 2) * (width / 2) + (x / 2);

            // 取相邻4个UV像素（实际是2x2 Y对应的UV）
            unsigned char u1 = u_422[pos_422];
            unsigned char u2 = u_422[pos_422 + 1];
            unsigned char u3 = u_422[pos_422 + width / 2];
            unsigned char u4 = u_422[pos_422 + width / 2 + 1];

            unsigned char v1 = v_422[pos_422];
            unsigned char v2 = v_422[pos_422 + 1];
            unsigned char v3 = v_422[pos_422 + width / 2];
            unsigned char v4 = v_422[pos_422 + width / 2 + 1];

            // 计算平均值（简单相加/4，避免浮点运算）
            u_420[pos_420] = (u1 + u2 + u3 + u4) / 4;
            v_420[pos_420] = (v1 + v2 + v3 + v4) / 4;
        }
    }

        // Write the modified YUV data to the output file
    if (fwrite(yuv420p, 1, frm_len_420, trans_ctx->fp_out) != frm_len_420) {
        perror("fwrite");
        return RET_NOK;
    }

    return ret;
}

static RET trans_yuv_fmt_process(TransYuvCtx *trans_ctx)
{
    RET ret = RET_OK;

    PixFmt in_fmt = trans_ctx->calc_ctx->input_fmt;
    PixFmt out_fmt = trans_ctx->calc_ctx->output_fmt;

    if (in_fmt == PIX_FMT_YUV420SP && out_fmt == PIX_FMT_YUV420P) {
        // Convert YUV420SP to YUV420P
        ret = trans_yuv420sp_to_yuv420p(trans_ctx);
    } else if (in_fmt == PIX_FMT_YUV420P && out_fmt == PIX_FMT_YUV420SP) {
        ret = trans_yuv420p_to_yuv420sp(trans_ctx);
    } else if (in_fmt == PIX_FMT_YUV422P && out_fmt == PIX_FMT_YUV420P) {
        ret = trans_yuv422p_to_yuv420p(trans_ctx);
    } else if (in_fmt == PIX_FMT_YUV420P && out_fmt == PIX_FMT_YUV422P) {

    } else if (in_fmt == PIX_FMT_YUV420SP && out_fmt == PIX_FMT_YUV420SP) {
    }

    return ret;
}

RET transform_yuv_format(CalcCtx *ctx)
{
    uint32_t frm_cnt = 0;
    TransYuvCtx trans_ctx;
    memset(&trans_ctx, 0, sizeof(TransYuvCtx));

    trans_yuv_fmt_init(ctx, &trans_ctx);

    do {
        if (trans_yuv_fmt_process(&trans_ctx)) {
            printf("Error processing frame %d\n", frm_cnt);
            break;
        }
    } while(++frm_cnt < ctx->frames);

    trans_yuv_fmt_deinit(&trans_ctx);

    printf("transform_yuv_format processed %d frames\n", frm_cnt);

    return RET_OK;
}