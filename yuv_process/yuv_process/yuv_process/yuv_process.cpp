#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <vector>
#include "getopt.hpp"
#include "rectangle.h"

using namespace std;

#ifdef _WIN32
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif

typedef struct {
    uint32_t frame_cnt;
    uint32_t mb_size;
    uint32_t mb_width;
    uint32_t mb_height;
    uint32_t mb_x;
    uint32_t mb_y;
} SadInfo;

static void draw_red_dot(YuvInfo *yuv, SadInfo *info)
{
    RectangleInfo rec_info;
    rec_info.left = info->mb_x * info->mb_size;
    rec_info.top = info->mb_y * info->mb_size;
    rec_info.right = info->mb_x * info->mb_size + 1;
    rec_info.bottom = info->mb_y * info->mb_size + 1;
    rec_info.y_pixel = 76; /* Red */
    rec_info.u_pixel = 84;
    rec_info.v_pixel = 255;

    draw_rectangle(yuv, &rec_info);
}

static void rk_handle_md(YuvInfo *yuv, ifstream *sad, SadInfo *info, uint32_t frame_num)
{
    vector<Rect> rects;
    Rect rect;
    char lines[512];
    if (frame_num == 1 && sad->getline(lines, 512)) {
        cout << lines << endl;
        int match_cnt = SSCANF(lines, "frame=%d mb_size=%d mb_width=%d mb_height=%d mb_x=%d mb_y=%d",
                               &info->frame_cnt, &info->mb_size, &info->mb_width, &info->mb_height, &info->mb_x, &info->mb_y);
        if (match_cnt > 1) {
            cout << "match_cnt " << match_cnt << " frame_cnt " << info->frame_cnt
                 << " mb_width " << info->mb_width << " mb_height " << info->mb_height
                 << " mb_x " << info->mb_x << " mb_y " << info->mb_y << endl;
        }
    }

    while (info->frame_cnt == frame_num) {
        draw_red_dot(yuv, info);

        rect.left = info->mb_x * info->mb_size;
        rect.top = info->mb_y * info->mb_size;
        rect.right = rect.left + info->mb_size;
        rect.bottom = rect.top + info->mb_size;
        rects.push_back(rect);

        if (sad->getline(lines, 512)) {
            //cout << lines << endl;
            int match_cnt = SSCANF(lines, "frame=%d mb_size=%d mb_width=%d mb_height=%d mb_x=%d mb_y=%d",
                                   &info->frame_cnt, &info->mb_size, &info->mb_width, &info->mb_height, &info->mb_x, &info->mb_y);
            if (match_cnt > 1) {
                //cout << "match_cnt " << match_cnt << " frame_cnt " << info->frame_cnt
                //     << " mb_width " << info->mb_width << " mb_height " << info->mb_height
                //     << " mb_x " << info->mb_x << " mb_y " << info->mb_y << endl;
            }
        } else {
            cout << "No sad info now, exit!" << endl;
            break;
        }
    }

    cout << "frame_num " << frame_num << " Vector Rect Number " << rects.size() << endl;

    /*  vector<Rect>::iterator iter;
      for (iter = rects.begin(); iter != rects.end(); iter++) {
          cout << iter->left << " " << iter->top << " "
               << iter->right << " " << iter->bottom << endl;
      }*/
    merge_rect(rects);
    cout << "frame_num " << frame_num << " finish merge" << endl;

    draw_blue_rectangle(yuv, rects);

}

int main(int argc, char **argv)
{
    cout << "----------Test-------------" << endl;
    bool help = getarg(false, "-H", "--help", "-?");
    string in_file = getarg("F:\\rkvenc_verify\\input_yuv\\3903_720x576.yuv", "-i", "--input");
    string out_file = getarg("F:\\rkvenc_verify\\input_yuv\\3903_720x576_hi_rk.yuv", "-o", "--output");
    string coord_file = getarg("F:\\rkvenc_verify\\input_yuv\\3903.md", "-c", "--coordinate");
    string sad_file = getarg("F:\\rkvenc_verify\\input_yuv\\3903_720x576_150.sad", "-s", "--sad");
    uint32_t width = getarg(720, "-w", "--width");
    uint32_t height = getarg(576, "-h", "--height");
    uint32_t left = getarg(10, "-l", "--left");
    uint32_t top = getarg(20, "-t", "--top");
    uint32_t right = getarg(50, "-r", "--right");
    uint32_t bottom = getarg(80, "-b", "--bottom");
    uint32_t frames = getarg(2, "-f", "--frames");
    uint8_t enable_draw_dot = getarg(1, "-dd", "--draw_dot");

    if (help) {
        cout << "Usage:" << endl
             << "yuv_process.exe -i=in.yuv -o=out.yuv "
             << "-w width -h height"
             << endl;
    }

    unsigned int luma_size = width * height;
    unsigned int chroma_size = luma_size / 2;
    unsigned int frame_size = luma_size + chroma_size;
    char *buf = new char[frame_size];
    unsigned int idx = 0;

    cout << "input: " << in_file << endl;
    string in_path = in_file;
    istream *ifs = new ifstream(in_path.c_str(), ios::binary | ios::in);

    cout << "output: " << out_file << endl;
    string out_path = out_file;
    ofstream ofs;
    ofs.open(out_path.c_str(), ios::binary | ios::out);

    unsigned int frame_cnt, region_num, region_idx;
    ifstream coord(coord_file.c_str());
    char lines[512];
    if (coord.getline(lines, 512)) {
        cout << lines << endl;

        int match_cnt = SSCANF(lines, "frame=%d, num=%d, idx=%d, left=%d, top=%d, right=%d, bottom=%d",
                               &frame_cnt, &region_num, &region_idx, &left, &top, &right, &bottom);
        if (match_cnt > 1) {
            cout << "match_cnt " << match_cnt << " frame_cnt " << frame_cnt
                 << " region_num " << region_num << " region_idx " << region_idx
                 << " left " << left << " top " << top
                 << " right " << right << " bottom " << bottom << endl;
        }
    }

    cout << "sad path: " << sad_file << endl;
    ifstream sad_path(sad_file.c_str());
    unsigned int frame_read = 0;
    YuvInfo yuv_info;
    RectangleInfo rec_info;
    SadInfo sad_info;
    ifs->read(buf, frame_size);

    do {
        while (frame_read == frame_cnt) {
            yuv_info.buf = buf;
            yuv_info.width = width;
            yuv_info.height = height;
            rec_info.left = left;
            rec_info.top = top;
            rec_info.right = right;
            rec_info.bottom = bottom;
            rec_info.y_pixel = 76; /* Red */
            rec_info.u_pixel = 84;
            rec_info.v_pixel = 255;

            draw_rectangle(&yuv_info, &rec_info);

            if (coord.getline(lines, 512)) {
                //cout << lines << endl;

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

        if (enable_draw_dot && frame_read > 0) {
            rk_handle_md(&yuv_info, &sad_path, &sad_info, frame_read);
        }

        ofs.write(buf, frame_size);

        ifs->read(buf, frame_size);
        frame_read++;
    } while (frame_read < frames);

    if (ifs && ifs != &cin)
        delete ifs;
    ofs.close();

    if (buf)
        delete [] buf;

    cout << "----------End!-------------" << endl;
    //string str;
    //cin >> str;
    return 0;
}
