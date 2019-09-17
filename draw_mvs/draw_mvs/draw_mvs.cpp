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



static void process_yuv(ProcCtx *ctx)
{
    uint32_t luma_size = ctx->width * ctx->height;
    uint32_t chroma_size = luma_size / 2;
    uint32_t frame_size = luma_size + chroma_size;
    uint32_t left = ctx->left;
    uint32_t top = ctx->top;
    uint32_t right = ctx->right;
    uint32_t bottom = ctx->bottom;
    uint32_t frame_cnt, region_num, region_idx;
    ifstream coord(ctx->coord_file.c_str());
    ifstream sad_path(ctx->sad_file.c_str());
    YuvInfo *yuv_info = &ctx->yuv_info;
    RectangleInfo rec_info;
    char lines[512];
    char *buf = new char[frame_size];

    if (coord.getline(lines, 512)) {
        int match_cnt = SSCANF(lines, "frame=%d, num=%d, idx=%d, left=%d, top=%d, right=%d, bottom=%d",
                               &frame_cnt, &region_num, &region_idx, &left, &top, &right, &bottom);
        if (match_cnt > 1) {
            cout << "match_cnt " << match_cnt << " frame_cnt " << frame_cnt
                 << " region_num " << region_num << " region_idx " << region_idx
                 << " left " << left << " top " << top
                 << " right " << right << " bottom " << bottom << endl;
        }
    }

    ctx->ifs->read(buf, frame_size);

    do {
        while (ctx->frame_read == frame_cnt) {
            yuv_info->buf = buf;
            yuv_info->width = ctx->width;
            yuv_info->height = ctx->height;
            rec_info.left = left;
            rec_info.top = top;
            rec_info.right = right;
            rec_info.bottom = bottom;
            rec_info.y_pixel = 76; /* Red */
            rec_info.u_pixel = 84;
            rec_info.v_pixel = 255;

            // TODO: draw arrows


            if (coord.getline(lines, 512)) {
                int match_cnt = SSCANF(lines, "frame=%d, num=%d, idx=%d, left=%d, top=%d, right=%d, bottom=%d",
                                       &frame_cnt, &region_num, &region_idx, &left, &top, &right, &bottom);
                if (match_cnt > 1) {
                    //cout << "match_cnt " << match_cnt << " frame_cnt " << frame_cnt
                    //     << " region_num " << region_num << " region_idx " << region_idx
                    //     << " left " << left << " top " << top
                    //     << " right " << right << " bottom " << bottom << endl;
                }
            } else {
                cout << "No MD info now, exit!" << endl;
                break;
            }
            cout << "finish frame " << frame_cnt
                 << " region " << region_idx << endl;
        }

        ctx->ofs->write(buf, frame_size);

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
    string in_file = getarg("F:\\rkvenc_verify\\input_yuv\\3903_720x576.yuv", "-i", "--input");
    string out_file = getarg("F:\\rkvenc_verify\\input_yuv\\3903_720x576_hi_rk.yuv", "-o", "--output");
    ctx->coord_file = getarg("F:\\rkvenc_verify\\cfg\\3903_hisilicon.md", "-c", "--coordinate");
    ctx->sad_file = getarg("F:\\rkvenc_verify\\cfg\\3903_720x576_150_1.sad", "-s", "--sad");
    ctx->width = getarg(720, "-w", "--width");
    ctx->height = getarg(576, "-h", "--height");
    ctx->left = getarg(10, "-l", "--left");
    ctx->top = getarg(20, "-t", "--top");
    ctx->right = getarg(50, "-r", "--right");
    ctx->bottom = getarg(80, "-b", "--bottom");
    ctx->frames = getarg(2, "-f", "--frames");
    ctx->motion_rate_thresh = getarg(50, "-m", "--motion_thresh");
    ctx->enable_draw_dot = getarg(1, "-dd", "--draw_dot");
    ctx->draw_blue_dot = getarg(1, "-dbd", "--draw_blue_dot");
    ctx->draw_blue_rect = getarg(0, "-dbr", "--draw_blue_rect");

    if (help) {
        cout << "Usage:" << endl
             << "./yuv_process -i=3903_720x576.yuv -o=3903_720x576_hi_rk.yuv "
             << "-c=3903.md -s=3903_720x576_150.sad -f=2"
             << endl;
        return 0;
    }

    cout << "input: " << in_file << endl
         << "output: " << out_file << endl;
    ctx->ifs = new ifstream(in_file.c_str(), ios::binary | ios::in);

    ofstream ofs;
    ctx->ofs = &ofs;
    ofs.open(out_file.c_str(), ios::binary | ios::out);

    process_yuv(ctx);

    if (ctx->ifs && ctx->ifs != &cin)
        delete ctx->ifs;
    ofs.close();

    cout << "----------End!-------------" << endl;
    //string str;
    //cin >> str;
    return 0;
}
