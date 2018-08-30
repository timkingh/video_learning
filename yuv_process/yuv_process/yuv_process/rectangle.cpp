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
    uint32_t motion_rate = 1, motion_rate_thresh = 50;

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

























