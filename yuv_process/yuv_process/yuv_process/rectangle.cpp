#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include "rectangle.h"
#include "yuv_process.h"

using namespace std;

#define MB_SIZE    (4)

inline bool Rect::operator == (const Rect &rect) const
{
    if (left == rect.left && top == rect.top &&
        right == rect.right && bottom == rect.bottom)
        return true;
    return false;
}

void Rect::calculate_area()
{
    area = (right - left) * (bottom - top);
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

void display_rect(Rect *rect)
{
    cout << " (" << setw(3) << rect->left << "," << setw(3) << rect->top
         << "," << setw(3) << rect->right << "," << setw(3) << rect->bottom << ") ("
         << setw(6) << rect->area << "," << setw(3) << rect->motion_rate << ") ";
}

void display_rects(vector<Rect> &rects)
{
    uint32_t i = 0;
    for (i = 0; i < rects.size(); i++) {
        cout << "rectangle " << setw(3) << i << ": ";
        display_rect(&(rects.at(i)));
        cout << endl;
    }
  /*  vector<Rect>::iterator iter;
    for (iter = rects.begin(); iter < rects.end(); iter++) {
        cout << "rectangle " << setw(3) << idx++ << ": " << setw(3) << iter->left << " " << iter->top
             << " " << iter->right << " " << iter->bottom << endl;
    }*/
}

static void calculate_distance(vector<Rect> &rects)
{
    vector<Rect>::iterator iter = rects.begin();
    uint32_t base_left = iter->left;
    uint32_t base_top = iter->top;

    for (iter; iter != rects.end(); iter++) {
        int32_t x_diff = iter->left - base_left;
        int32_t y_diff = iter->top - base_top;
        iter->distance = x_diff * x_diff + y_diff * y_diff;
    }
}

static bool compare_area(const Rect &a, const Rect &b)
{
    return (a.area > b.area);
}

static bool compare_dist(const Rect &a, const Rect &b)
{
    return (a.distance < b.distance);
}

static void sort_rects(vector<Rect> groups[])
{
    uint32_t idx;

    for (idx = 0; idx < 4; idx++) {
        calculate_distance(groups[idx]);
        sort(groups[idx].begin(), groups[idx].end(), compare_dist);
    }
}

void calc_rects_average(vector<Rect> &rects, Rect *ave)
{
    Rect sum;
    sum.left = sum.top = sum.right = sum.bottom = 0;

    vector<Rect>::iterator iter;
    for (iter = rects.begin(); iter != rects.end(); iter++) {
        sum.left += iter->left;
        sum.top += iter->top;
        sum.right += iter->right;
        sum.bottom += iter->bottom;
    }

    ave->left = sum.left / rects.size();
    ave->top = sum.top / rects.size();
    ave->right = sum.right / rects.size();
    ave->bottom = sum.bottom / rects.size();

    cout << "average region: ";
    display_rect(ave);
    cout << endl;
}

void divide_rects_into_groups(vector<Rect> &rects, Rect *ave, vector<Rect> groups[])
{
    uint32_t total_num = rects.size();
    uint32_t idx, rects_idx;
    vector<Rect>::iterator iter;

    for (idx = 0; idx < total_num; idx++) {
        iter = rects.begin();

        if (iter->left <= ave->left && iter->top <= ave->top) {
            rects_idx = 0;
        } else if (iter->left > ave->left && iter->top <= ave->top) {
            rects_idx = 1;
        } else if (iter->left <= ave->left && iter->top > ave->top) {
            rects_idx = 2;
        } else {
            rects_idx = 3;
        }

        groups[rects_idx].push_back(rects.at(0));
        rects.erase(iter);
    }

    cout << "After division " << setw(4) << groups[0].size()
        << setw(4) << groups[1].size()
        << setw(4) << groups[2].size()
        << setw(4) << groups[3].size() << endl;
}

void handle_horizontal_rects(vector<uint32_t> &xs, uint32_t y_min, uint32_t y_max, vector<Rect> &rects)
{
    vector<uint32_t> all_x = xs;
    uint32_t num = all_x.size() / 2;

    sort(all_x.begin(), all_x.end());
    all_x.erase(unique(all_x.begin(), all_x.end()), all_x.end());

    for (uint32_t i = 0; i < all_x.size() - 1; i++) {
        uint32_t low = all_x[i], high = all_x[i + 1];

        for (uint32_t j = 0; j < num; j++) {
            if (xs[j * 2] <= low && high <= xs[j * 2 + 1]) {
                Rect rect;
                rect.left = low;
                rect.right = high;
                rect.top = y_min;
                rect.bottom = y_max;

                rects.push_back(rect);
            }
        }
    }
}

void handle_vertical_rects(vector<uint32_t> &all_y, vector<Rect> &rects_org, vector<Rect> &rects)
{
    uint32_t i, j;
    vector<uint32_t> xs;
    Rect rect;

    rects.clear(); /* used for saving new rects */

    for (i = 0; i < all_y.size(); i++) {
        uint32_t y_min = all_y.at(i);
        uint32_t y_max = all_y.at(i + 1);

        for (j = 0; j < rects_org.size(); j++) {
            rect = rects_org.at(j);

            if (rect.top <= y_min && y_max <= rect.bottom) {
                xs.push_back(rect.left);
                xs.push_back(rect.right);
            }
        }

        if (xs.size() != 0) {
			handle_horizontal_rects(xs, y_min, y_max, rects);
        }
    }
}

void make_rect_unique(vector<Rect> &rects)
{
    vector<uint32_t> all_y;
    vector<Rect>::iterator iter;
    for (iter = rects.begin(); iter != rects.end(); iter++) {
        all_y.push_back(iter->top);
        all_y.push_back(iter->bottom);
    }

    sort(all_y.begin(), all_y.end());
    all_y.erase(unique(all_y.begin(), all_y.end()), all_y.end());

    vector<Rect> rects_org = rects;
    handle_vertical_rects(all_y, rects_org, rects);
}

void merge_groups_into_rects(vector<Rect> groups[], vector<Rect> &rects_new)
{
    uint32_t idx, rects_idx;
    for (rects_idx = 0; rects_idx < 4; rects_idx++) {
        for (idx = 0; idx < groups[rects_idx].size(); idx++) {
            rects_new.push_back(groups[rects_idx].at(idx));
        }
    }

    cout << "new rects " << rects_new.size() << endl;
}

void merge_rect(void *proc_ctx, vector<Rect> &rects, vector<Rect> &rects_org)
{
    ProcCtx *ctx = (ProcCtx *)proc_ctx;
    Rect dst;
    uint32_t i, j;
    uint32_t motion_rate = 1, merge_cnt = 1;
    uint32_t motion_rate_thresh = ctx->motion_rate_thresh;

run_again:
    for (i = 0; i < rects.size() - 1; i++) {
        for (j = i + 1; j < rects.size(); j++) {
            merge_two_rect(rects.at(i), rects.at(j), dst);

            motion_rate = calc_motion_rate(rects_org, dst);
            if (motion_rate >= motion_rate_thresh) {
                Rect a = rects.at(i);
                Rect b = rects.at(j);
                //display_rect(&a);
                //display_rect(&b);
                cout << "merge cnt " << setw(4) << merge_cnt++
                     << " motion rate " << setw(3) << motion_rate << endl;

                vector<Rect>::iterator iter;
                iter = find(rects.begin(), rects.end(), a);
                rects.erase(iter);
                iter = find(rects.begin(), rects.end(), b);
                rects.erase(iter);

                dst.motion_rate = motion_rate;
                dst.calculate_area();
                rects.push_back(dst);

                goto run_again;
            }
        }
    }

    /* sort rects in area, descending order */
    sort(rects.begin(), rects.end(), compare_area);
    cout << "After merge, vector rect number " << rects.size() << endl;
    display_rects(rects);
}

void merge_rect_optimize(void *proc_ctx, vector<Rect> &rects_org, vector<Rect> &rects_new)
{
    vector<Rect> rects = rects_org;
    uint32_t idx;
    Rect ave_rect;

    calc_rects_average(rects, &ave_rect);

    vector<Rect> groups[4];
    divide_rects_into_groups(rects, &ave_rect, groups);

    sort_rects(groups);

    vector<Rect> groups_org[4];
    for (idx = 0; idx < 4; idx++) {
        groups_org[idx] = groups[idx];
        merge_rect(proc_ctx, groups[idx], groups_org[idx]);
    }

    merge_groups_into_rects(groups, rects_new);

    merge_rect(proc_ctx, rects_new, rects_org);

    make_rect_unique(rects_new);
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
    uint32_t idx = 0, rects_size = rects.size();
    RectangleInfo rec_info;
    rec_info.y_pixel = 29; /* Blue */
    rec_info.u_pixel = 255;
    rec_info.v_pixel = 107;

    for (idx = 0; idx < rects_size; idx++) {
        Rect rect = rects.at(idx);
        rec_info.left = rect.left;
        rec_info.top = rect.top;
        rec_info.right = rect.right;
        rec_info.bottom = rect.bottom;

        draw_rectangle(yuv, &rec_info);
    }
}























