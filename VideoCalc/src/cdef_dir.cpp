#include "video_common.h"

/* Detect direction. 0 means 45-degree up-right, 2 is horizontal, and so on.
   The search minimizes the weighted variance along all the lines in a
   particular direction, i.e. the squared error between the input and a
   "predicted" block where each pixel is replaced by the average along a line
   in a particular direction. Since each direction have the same sum(x^2) term,
   that term is never computed. See Section 2, step 2, of:
   http://jmvalin.ca/notes/intra_paint.pdf */
static int cdef_find_dir_c(uint8_t *img, int stride, int32_t *var,
                           int coeff_shift) {
  int i;
  int32_t cost[8] = { 0 };
  int partial[8][15] = { { 0 } };
  int32_t best_cost = 0;
  int best_dir = 0;
  /* Instead of dividing by n between 2 and 8, we multiply by 3*5*7*8/n.
     The output is then 840 times larger, but we don't care for finding
     the max. */
  static const int div_table[] = { 0, 840, 420, 280, 210, 168, 140, 120, 105 };
  for (i = 0; i < 8; i++) {
    int j;
    for (j = 0; j < 8; j++) {
      int x;
      /* We subtract 128 here to reduce the maximum range of the squared
         partial sums. */
      x = (img[i * stride + j] >> coeff_shift) - 128;
      partial[0][i + j] += x;
      partial[1][i + j / 2] += x;
      partial[2][i] += x;
      partial[3][3 + i - j / 2] += x;
      partial[4][7 + i - j] += x;
      partial[5][3 - i / 2 + j] += x;
      partial[6][j] += x;
      partial[7][i / 2 + j] += x;
    }
  }
  for (i = 0; i < 8; i++) {
    cost[2] += partial[2][i] * partial[2][i];
    cost[6] += partial[6][i] * partial[6][i];
  }
  cost[2] *= div_table[8];
  cost[6] *= div_table[8];
  for (i = 0; i < 7; i++) {
    cost[0] += (partial[0][i] * partial[0][i] +
                partial[0][14 - i] * partial[0][14 - i]) *
               div_table[i + 1];
    cost[4] += (partial[4][i] * partial[4][i] +
                partial[4][14 - i] * partial[4][14 - i]) *
               div_table[i + 1];
  }
  cost[0] += partial[0][7] * partial[0][7] * div_table[8];
  cost[4] += partial[4][7] * partial[4][7] * div_table[8];
  for (i = 1; i < 8; i += 2) {
    int j;
    for (j = 0; j < 4 + 1; j++) {
      cost[i] += partial[i][3 + j] * partial[i][3 + j];
    }
    cost[i] *= div_table[8];
    for (j = 0; j < 4 - 1; j++) {
      cost[i] += (partial[i][j] * partial[i][j] +
                  partial[i][10 - j] * partial[i][10 - j]) *
                 div_table[2 * j + 2];
    }
  }
  for (i = 0; i < 8; i++) {
    if (cost[i] > best_cost) {
      best_cost = cost[i];
      best_dir = i;
    }
  }
  /* Difference between the optimal variance and the variance along the
     orthogonal direction. Again, the sum(x^2) terms cancel out. */
  *var = best_cost - cost[(best_dir + 4) & 7];
  /* We'd normally divide by 840, but dividing by 1024 is close enough
     for what we're going to do with this. */
  *var >>= 10;
  return best_dir;
}

RET cdef_find_dir(CalcCtx *ctx)
{
    const char *input_file = ctx->input.c_str();
    FILE *fp_yuv_in;
    FILE *fp_log_out;
    int k, blk_var, dir, b8_idx;
    int width = ctx->width, height = ctx->height;
    int b16_w = width / 16, b16_h = height / 16;

    fp_yuv_in = fopen(input_file, "rb");
    if (fp_yuv_in == NULL) {
        perror("fopen input");
        return RET_NOK;
    }

    fp_log_out = fopen(ctx->output.c_str(), "ab");
    if (fp_log_out == NULL) {
        perror("fopen output");
        return RET_NOK;
    }

    for (k = 0; k < (int)ctx->frames; k++) {
        read_frame_from_file(ctx, fp_yuv_in, (uint8_t *)ctx->frm_buf0);

        for (int b16_y = 0; b16_y < b16_h; b16_y++) {
            for (int b16_x = 0; b16_x < b16_w; b16_x++) {
                for (b8_idx = 0; b8_idx < 4; b8_idx++) {
                    int pos_x = b16_x * 16 + (b8_idx % 2) * 8;
                    int pos_y = b16_y * 16 + (b8_idx / 2) * 8;
                    uint8_t * src = (uint8_t *)ctx->frm_buf0 + pos_x + pos_y * width;

                    dir = cdef_find_dir_c(src, ctx->width, &blk_var, 0);
                    // FPRINT(fp_log_out, "%d\n", dir);
                    FPRINT(fp_log_out, "pos(%d, %d) dir %d var %d\n",
                           pos_x, pos_y, dir, blk_var);
                }
            }
        }
    }

    FPCLOSE(fp_yuv_in);
    FPCLOSE(fp_log_out);

    printf("cdef_dir finished %d frames(s)\n", ctx->frames);

    return RET_OK;
}
