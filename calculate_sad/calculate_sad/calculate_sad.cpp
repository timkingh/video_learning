#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <stdint.h>
#include "getopt.hpp"

using namespace std;

typedef struct {
    char *buf;
    unsigned int width;
    unsigned int height;
} YuvInfo;

typedef struct {
    unsigned int left;
    unsigned int top;
    unsigned int right;
    unsigned int bottom;
} RectangleInfo;

static void draw_red_rectangle(YuvInfo *yuv, RectangleInfo *rec)
{
    unsigned int idx;
    unsigned int x0, y0, x1, y1;
    unsigned int width = yuv->width;
    unsigned int height = yuv->height;
    char *buf_y = yuv->buf;
    char *buf_u = yuv->buf + width * height;
    char *buf_v = buf_u + width * height / 4;
    x0 = rec->left;
    y0 = rec->top;
    x1 = rec->right;
    y1 = rec->bottom;

    /* Y */
    char *buf_top = buf_y + x0 + y0 * width;
    char *buf_bottom = buf_y + x0 + y1 * width;
    for (idx = 0; idx < x1 - x0 + 1; idx++) {
        buf_top[idx] = buf_bottom[idx] = 76;
    }

    char *buf_left = buf_top;
    char *buf_right = buf_left + (x1 - x0);
    for (idx = 0; idx < y1 - y0 + 1; idx++) {
        buf_left[idx * width] = buf_right[idx * width] = 76;
    }

    unsigned int c_x0, c_y0, c_x1, c_y1;
    unsigned int chroma_width = width / 2;
    c_x0 = x0 / 2;
    c_y0 = y0 / 2;
    c_x1 = x1 / 2;
    c_y1 = y1 / 2;

    /* U */
    buf_top = buf_u + c_x0 + c_y0 * chroma_width;
    buf_bottom = buf_u + c_x0 + c_y1 * chroma_width;
    for (idx = 0; idx < c_x1 - c_x0 + 1; idx++) {
        buf_top[idx] = buf_bottom[idx] = 84;
    }

    buf_left = buf_top;
    buf_right = buf_left + (c_x1 - c_x0);
    for (idx = 0; idx < c_y1 - c_y0 + 1; idx++) {
        buf_left[idx * chroma_width] = buf_right[idx * chroma_width] = 84;
    }

    /* V */
    buf_top = buf_v + c_x0 + c_y0 * chroma_width;
    buf_bottom = buf_v + c_x0 + c_y1 * chroma_width;
    for (idx = 0; idx < c_x1 - c_x0 + 1; idx++) {
        buf_top[idx] = buf_bottom[idx] = 255;
    }

    buf_left = buf_top;
    buf_right = buf_left + (c_x1 - c_x0);
    for (idx = 0; idx < c_y1 - c_y0 + 1; idx++) {
        buf_left[idx * chroma_width] = buf_right[idx * chroma_width] = 255;
    }
}

static int calc_sad(uint8_t *pix1, int stride_pix1, uint8_t *pix2, int stride_pix2, int num)
{
    int sum = 0;
    for (int y = 0; y < num; y++) {
        for (int x = 0; x < num; x++) {
            sum += abs(pix1[x] - pix2[x]);
        }
        pix1 += stride_pix1;
        pix2 += stride_pix2;
    }
    return sum;
}

void main(int argc, char **argv)
{
    cout << "----------Test-------------" << endl;
    bool help = getarg(false, "-H", "--help", "-?");
    string in_file = getarg("F:\\rkvenc_verify\\input_yuv\\3903_720x576.yuv", "-i", "--input");
    string out_file = getarg("F:\\rkvenc_verify\\input_yuv\\3903_720x576.sad", "-o", "--output");
    string coord_file = getarg("F:\\rkvenc_verify\\input_yuv\\out.md", "-c", "--coordinate");
    unsigned int width = getarg(720, "-w", "--width");
    unsigned int height = getarg(576, "-h", "--height");
    unsigned int cu_size = getarg(4, "-s", "--cu_size");
    unsigned int threshold = getarg(100, "-t", "--threshold");
    unsigned int right = getarg(50, "-r", "--right");
    unsigned int bottom = getarg(80, "-b", "--bottom");
    unsigned int frames = getarg(5, "-f", "--frames");

    if (help) {
        cout << "Usage:" << endl
             << "yuv_process.exe -i=in.yuv -o=out.yuv "
             << "-w width -h height"
             << endl;
    }

    unsigned int frame_read = 0;
    unsigned int luma_size = width * height;
    unsigned int chroma_size = luma_size / 2;
    unsigned int frame_size = luma_size + chroma_size;
    char *buf[2] = {0};
    uint32_t buf_idx;
    buf[0] = new char[frame_size];
    buf[1] = new char[frame_size];

    cout << "input: " << in_file << endl;
    string in_path = in_file;
    istream *ifs = new ifstream(in_path.c_str(), ios::binary | ios::in);

    cout << "output: " << out_file << endl;
    string out_path = out_file;
    ofstream ofs;
    ofs.open(out_path.c_str(), ios::out);

    do {
        buf_idx = frame_read % 2;
        ifs->read(buf[buf_idx], frame_size);

        if (frame_read > 0) {
            uint32_t mb_size = cu_size;
            uint32_t mb_width = width / mb_size;
            uint32_t mb_height = height / mb_size;

            for (uint32_t y = 0; y < mb_height; y++) {
                for (uint32_t x = 0; x < mb_width; x++) {
                    uint32_t sad = 0;
                    uint8_t *buf0 = (uint8_t *)buf[0] + x * mb_size + y * mb_size * width;
                    uint8_t *buf1 = (uint8_t *)buf[1] + x * mb_size + y * mb_size * width;

                    sad = calc_sad(buf0, width, buf1, width, mb_size);

                    if (sad >= threshold) {
                        //ofs << setfill('0') << setw(3);
                        ofs << "frame=" << frame_read
                            << " mb_width=" << mb_width
                            << " mb_height=" << mb_height
                            << " mb_x=" << x
                            << " mb_y=" << y << endl;
                    }
                }
            }
        }

        frame_read++;
    } while (frame_read < frames);

    cout << "----------End!-------------" << endl;

    if (ifs && ifs != &cin)
        delete ifs;
    ofs.close();

    if (buf[0])
        delete[] buf[0];
    if (buf[1])
        delete[] buf[1];

    string str;
    cin >> str;
    return;
}





