#include <iostream>
#include <fstream>
#include <string.h>  /* needed by sscanf */
#include <string>
#include <vector>
#include "getopt.hpp"
#include "common.h"
#include "draw_mvs.h"

using namespace std;

#ifdef _WIN32
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif

/**
 * Absolute value, Note, INT_MIN / INT64_MIN result in undefined behavior as they
 * are not representable as absolute values of their type. This is the same
 * as with *abs()
 * @see FFNABS()
 */
#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))

#define FFSWAP(type,a,b) do{type SWAP_tmp= b; b= a; a= SWAP_tmp;}while(0)
/* assume b>0 */
#define ROUNDED_DIV(a,b) (((a)>0 ? (a) + ((b)>>1) : (a) - ((b)>>1))/(b))


/**
 * Clip a signed integer value into the amin-amax range.
 * @param a value to clip
 * @param amin minimum value of the clip range
 * @param amax maximum value of the clip range
 * @return clipped value
 */
static int av_clip(int a, int amin, int amax)
{
#if defined(HAVE_AV_CONFIG_H) && defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
    if (amin > amax) abort();
#endif
    if      (a < amin) return amin;
    else if (a > amax) return amax;
    else               return a;
}

static int clip_line(int *sx, int *sy, int *ex, int *ey, int maxx)
{
    if(*sx > *ex)
        return clip_line(ex, ey, sx, sy, maxx);

    if (*sx < 0) {
        if (*ex < 0)
            return 1;
        *sy = *ey + (*sy - *ey) * (int64_t)*ex / (*ex - *sx);
        *sx = 0;
    }

    if (*ex > maxx) {
        if (*sx > maxx)
            return 1;
        *ey = *sy + (*ey - *sy) * (int64_t)(maxx - *sx) / (*ex - *sx);
        *ex = maxx;
    }
    return 0;
}

/**
 * Draw a line from (ex, ey) -> (sx, sy).
 * @param w width of the image
 * @param h height of the image
 * @param stride stride/linesize of the image
 * @param color color of the arrow
 */
static void draw_line(uint8_t *buf, int sx, int sy, int ex, int ey,
                      int w, int h, int stride, int color)
{
    int x, y, fr, f;

    if (clip_line(&sx, &sy, &ex, &ey, w - 1))
        return;
    if (clip_line(&sy, &sx, &ey, &ex, h - 1))
        return;

    sx = av_clip(sx, 0, w - 1);
    sy = av_clip(sy, 0, h - 1);
    ex = av_clip(ex, 0, w - 1);
    ey = av_clip(ey, 0, h - 1);

    buf[sy * stride + sx] += color;

    if (FFABS(ex - sx) > FFABS(ey - sy)) {
        if (sx > ex) {
            FFSWAP(int, sx, ex);
            FFSWAP(int, sy, ey);
        }
        buf += sx + sy * stride;
        ex  -= sx;
        f    = ((ey - sy) << 16) / ex;
        for (x = 0; x <= ex; x++) {
            y  = (x * f) >> 16;
            fr = (x * f) & 0xFFFF;
                   buf[ y      * stride + x] += (color * (0x10000 - fr)) >> 16;
            if(fr) buf[(y + 1) * stride + x] += (color *            fr ) >> 16;
        }
    } else {
        if (sy > ey) {
            FFSWAP(int, sx, ex);
            FFSWAP(int, sy, ey);
        }
        buf += sx + sy * stride;
        ey  -= sy;
        if (ey)
            f = ((ex - sx) << 16) / ey;
        else
            f = 0;
        for(y= 0; y <= ey; y++){
            x  = (y*f) >> 16;
            fr = (y*f) & 0xFFFF;
                   buf[y * stride + x    ] += (color * (0x10000 - fr)) >> 16;
            if(fr) buf[y * stride + x + 1] += (color *            fr ) >> 16;
        }
    }
}


/**
 * Draw an arrow from (ex, ey) -> (sx, sy).
 * @param w width of the image
 * @param h height of the image
 * @param stride stride/linesize of the image
 * @param color color of the arrow
 */
static void draw_arrow(uint8_t *buf, int sx, int sy, int ex,
                       int ey, int w, int h, int stride, int color, int tail, int direction)
{
    int dx,dy;

    if (direction) {
        FFSWAP(int, sx, ex);
        FFSWAP(int, sy, ey);
    }

    sx = av_clip(sx, -100, w + 100);
    sy = av_clip(sy, -100, h + 100);
    ex = av_clip(ex, -100, w + 100);
    ey = av_clip(ey, -100, h + 100);

    dx = ex - sx;
    dy = ey - sy;

    if (dx * dx + dy * dy > 3 * 3) {
        int rx =  dx + dy;
        int ry = -dx + dy;
        int length = sqrt((rx * rx + ry * ry) << 8);

        // FIXME subpixel accuracy
        rx = ROUNDED_DIV(rx * 3 << 4, length);
        ry = ROUNDED_DIV(ry * 3 << 4, length);

        if (tail) {
            rx = -rx;
            ry = -ry;
        }

        draw_line(buf, sx, sy, sx + rx, sy + ry, w, h, stride, color);
        draw_line(buf, sx, sy, sx - ry, sy + rx, w, h, stride, color);
    }
    draw_line(buf, sx, sy, ex, ey, w, h, stride, color);
}

static void scale_mv(int *sx, int *sy, int *ex, int *ey, int cu_size)
{
    while ((*ex - *sx) * (*ex - *sx) + (*ey - *sy) * (*ey - *sy) > cu_size * cu_size) {
        *ex = (*ex + *sx + 1) / 2;
        *ey = (*ey + *sy + 1) / 2;
    }
}

static void draw_mvs_on_yuv(ProcCtx *ctx)
{
    uint32_t luma_size = ctx->width * ctx->height;
    uint32_t chroma_size = luma_size / 2;
    uint32_t frame_size = luma_size + chroma_size;
    uint32_t frame_cnt, cu_x, cu_y, cu_size;
    int32_t mv_x, mv_y, start_x, start_y, end_x, end_y;
    ifstream coord(ctx->coord_file.c_str());
    ifstream sad_path(ctx->sad_file.c_str());
    YuvInfo *yuv_info = &ctx->yuv_info;
    RectangleInfo rec_info;
    char lines[512];
    char *buf = new char[frame_size];

    if (coord.getline(lines, 512)) {
        int match_cnt = SSCANF(lines, "frame=%d, cu_x=%d, cu_y=%d, cu_size=%d, mv_x=%d, mv_y=%d",
                               &frame_cnt, &cu_x, &cu_y, &cu_size, &mv_x, &mv_y);
        if (match_cnt > 1) {
            cout << "match_cnt " << match_cnt << " frame_cnt " << frame_cnt
                 << " cu_x " << cu_x << " cu_y " << cu_y << " cu_size " << cu_size
                 << " mv_x " << mv_x << " mv_y " << mv_y << endl;
        }
    }

    ctx->ifs->read(buf, frame_size);

    do {
        const int direction = 0;//mv->source > 0;
        while (ctx->frame_read == frame_cnt) {
			if (frame_cnt > 0 && cu_size == ctx->cu_size) {
                start_x = cu_x * cu_size;
                start_y = cu_y * cu_size;
                end_x = start_x + mv_x;
                end_y = start_y + mv_y;
                //scale_mv(&start_x, &start_y, &end_x, &end_y, cu_size);
				draw_arrow((uint8_t *)buf, end_x, end_y, start_x, start_y, ctx->width, ctx->height, ctx->width, 100, 0, direction);
			}

            if (coord.getline(lines, 512)) {
                int match_cnt = SSCANF(lines, "frame=%d, cu_x=%d, cu_y=%d, cu_size=%d, mv_x=%d, mv_y=%d",
                                       &frame_cnt, &cu_x, &cu_y, &cu_size, &mv_x, &mv_y);
                if (match_cnt > 1) {
					//cout << "match_cnt " << match_cnt << " frame_cnt " << frame_cnt
					//	 << " cu_x " << cu_x << " cu_y " << cu_y << " cu_size " << cu_size
					//	 << " mv_x " << mv_x << " mv_y " << mv_y << endl;
                }
            } else {
                cout << "No MV info now, exit!" << endl;
                break;
            }
        }

        ctx->ofs->write(buf, frame_size);
        cout << "finished frame " << frame_cnt << endl;

        ctx->ifs->read(buf, frame_size);
        ctx->frame_read++;
    } while (ctx->frame_read < ctx->frames);

    delete [] buf;
}

int main(int argc, char **argv)
{
    ProcCtx proc_ctx;
    ProcCtx *ctx = &proc_ctx;
    memset(ctx, 0, sizeof(ProcCtx));

    /* -c 3903.md --- Motion detected regions of Hisilicon
     * -s   *.sad --- Motion detected regions of RK
     */
    cout << "----------Test-------------" << endl;
    bool help = getarg(false, "-H", "--help", "-?");
    string in_file = getarg("F:\\rkvenc_verify\\input_yuv\\yuv\\Kimono1_1920x1080_24.yuv", "-i", "--input");
    string out_file = getarg("F:\\rkvenc_verify\\debug\\1\\Kimono1_1920x1080_24_qp45_60frames_rime16_100_0.yuv", "-o", "--output");
    ctx->coord_file = getarg("F:\\rkvenc_verify\\debug\\1\\mei_kimonol_qp45_3frames_100_0.txt", "-c", "--coordinate");
    ctx->sad_file = getarg("F:\\rkvenc_verify\\cfg\\3903_720x576_150_1.sad", "-s", "--sad");
    ctx->width = getarg(1920, "-w", "--width");
    ctx->height = getarg(1080, "-h", "--height");
    ctx->left = getarg(10, "-l", "--left");
    ctx->top = getarg(20, "-t", "--top");
    ctx->right = getarg(50, "-r", "--right");
    ctx->bottom = getarg(80, "-b", "--bottom");
    ctx->frames = getarg(3, "-f", "--frames");
    ctx->cu_size = getarg(16, "--cu_size");
    ctx->enable_draw_dot = getarg(1, "-dd", "--draw_dot");
    ctx->draw_blue_dot = getarg(1, "-dbd", "--draw_blue_dot");
    ctx->draw_blue_rect = getarg(0, "-dbr", "--draw_blue_rect");

    if (help || argc < 2) {
        cout << "Usage:" << endl
             << "./draw_mvs -i=Kimono1_1920x1080_24.yuv -o=Kimono1_1920x1080_24_qp45_60frames_rime16_100_0.yuv "
             << "-c=mei_kimonol_qp45_3frames_100_0.txt -w=1920 -h=1080 --frames=60 --cu_size=16"
             << endl;
        return 0;
    }

    cout << "input: " << in_file << endl
         << "output: " << out_file << endl;
    ctx->ifs = new ifstream(in_file.c_str(), ios::binary | ios::in);

    ofstream ofs;
    ctx->ofs = &ofs;
    ofs.open(out_file.c_str(), ios::binary | ios::out);

    draw_mvs_on_yuv(ctx);

    if (ctx->ifs && ctx->ifs != &cin)
        delete ctx->ifs;
    ofs.close();

    cout << "----------End!-------------" << endl;
    //string str;
    //cin >> str;
    return 0;
}
