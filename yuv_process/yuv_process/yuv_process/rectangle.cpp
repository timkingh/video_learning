#include <iostream>
#include <string>
#include <algorithm>
#include "rectangle.h"

using namespace std;

#define MB_SIZE    (4)

inline bool Rect::operator == (const Rect &rect) const
{
    if (left == rect.left && top == rect.top &&
        right == rect.right && bottom == rect.bottom)
        return true;
    return false;
}

/* judge whether two rects intersect */
static bool is_intersect(Rect &rect1, Rect &rect2)
{
    return !(rect1.left >= rect2.right || rect2.left >= rect1.right || \
             rect1.top >= rect2.bottom || rect2.top >= rect1.bottom);
}

static void merge_two_rect(Rect &a, Rect &b, Rect &dst)
{
    dst.left = (a.left < b.left) ? a.left : b.left;
    dst.top = (a.top < b.top) ? a.top : b.top;
    dst.right = (a.right > b.right) ? a.right : b.right;
    dst.bottom = (a.bottom > b.bottom) ? a.bottom : b.bottom;
}

static uint32_t calc_motion_rate(vector<Rect> &rects, Rect &dst)
{
    uint32_t cnt = 0, rate = 1;
    uint32_t x, y;
    Rect cur_rect;

    for (y = dst.top; y < dst.bottom; y += MB_SIZE) {
        for (x = dst.left; x < dst.right; x += MB_SIZE) {
            cur_rect.left = x;
            cur_rect.top = y;
            cur_rect.right = x + MB_SIZE;
            cur_rect.bottom = y + MB_SIZE;

            uint32_t idx;
            for (idx = 0; idx < rects.size(); idx++) {
                if (is_intersect(rects.at(idx), cur_rect)) {
                    cnt++;
                    break;
                }
            }
        }
    }

    uint32_t region_width = (dst.right - dst.left) / MB_SIZE;
    uint32_t region_height = (dst.bottom - dst.top) / MB_SIZE;
    uint32_t total_mb = region_width * region_height;
    rate = cnt * 100 / total_mb;

    return rate;
}

void merge_rect(vector<Rect> &rects)
{
    vector<Rect> rects_org = rects;
    Rect dst;
    uint32_t i, j;
    uint32_t motion_rate = 1, motion_rate_thresh = 30;

run_again:
    for (i = 0; i < rects.size() - 1; i++) {
        for (j = i + 1; j < rects.size(); j++) {
            merge_two_rect(rects.at(i), rects.at(j), dst);

            motion_rate = calc_motion_rate(rects_org, dst);
            if (motion_rate >= motion_rate_thresh) {
                Rect a = rects.at(i);
                Rect b = rects.at(j);

                vector<Rect>::iterator iter;
                iter = find(rects.begin(), rects.end(), a);
                rects.erase(iter);
                iter = find(rects.begin(), rects.end(), b);
                rects.erase(iter);
                rects.push_back(dst);

                goto run_again;
            }
        }

    }

    cout << "After merge, vector rect number " << rects.size() << endl;
}

void draw_rectangle(YuvInfo *yuv, RectangleInfo *rec)
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
        buf_top[idx] = buf_bottom[idx] = rec->y_pixel;
    }

    char *buf_left = buf_top;
    char *buf_right = buf_left + (x1 - x0);
    for (idx = 0; idx < y1 - y0 + 1; idx++) {
        buf_left[idx * width] = buf_right[idx * width] = rec->y_pixel;
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
        buf_top[idx] = buf_bottom[idx] = rec->u_pixel;
    }

    buf_left = buf_top;
    buf_right = buf_left + (c_x1 - c_x0);
    for (idx = 0; idx < c_y1 - c_y0 + 1; idx++) {
        buf_left[idx * chroma_width] = buf_right[idx * chroma_width] = rec->u_pixel;
    }

    /* V */
    buf_top = buf_v + c_x0 + c_y0 * chroma_width;
    buf_bottom = buf_v + c_x0 + c_y1 * chroma_width;
    for (idx = 0; idx < c_x1 - c_x0 + 1; idx++) {
        buf_top[idx] = buf_bottom[idx] = rec->v_pixel;
    }

    buf_left = buf_top;
    buf_right = buf_left + (c_x1 - c_x0);
    for (idx = 0; idx < c_y1 - c_y0 + 1; idx++) {
        buf_left[idx * chroma_width] = buf_right[idx * chroma_width] = rec->v_pixel;
    }
}

void draw_blue_rectangle(YuvInfo *yuv, vector<Rect> &rects)
{
    uint32_t idx = 0;
    RectangleInfo rec_info;
    rec_info.y_pixel = 29; /* Blue */
    rec_info.u_pixel = 255;
    rec_info.v_pixel = 107;

    for (idx = 0; idx < rects.size(); idx++) {
        Rect &rect = rects.back();
        rec_info.left = rect.left;
        rec_info.top = rect.top;
        rec_info.right = rect.right;
        rec_info.bottom = rect.bottom;

        draw_rectangle(yuv, &rec_info);
        rects.pop_back();
    }
}























