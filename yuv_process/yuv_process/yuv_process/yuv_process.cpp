#include <iostream>
#include <fstream>
#include <string>

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

	unsigned int chroma_width = width / 2;
	buf_top = buf_u + x0 + y0 * chroma_width;
	buf_bottom = buf_u + x0 + y1 * chroma_width;
	for (idx = 0; idx < x1 - x0 + 1; idx++) {
		buf_top[idx] = buf_bottom[idx] = 84;
	}

	buf_left = buf_top;
	buf_right = buf_left + (x1 - x0);
	for (idx = 0; idx < y1 - y0 + 1; idx++) {
		buf_left[idx * chroma_width] = buf_right[idx * chroma_width] = 84;
	}

	buf_top = buf_v + x0 + y0 * chroma_width;
	buf_bottom = buf_v + x0 + y1 * chroma_width;
	for (idx = 0; idx < x1 - x0 + 1; idx++) {
		buf_top[idx] = buf_bottom[idx] = 255;
	}

	buf_left = buf_top;
	buf_right = buf_left + (x1 - x0);
	for (idx = 0; idx < y1 - y0 + 1; idx++) {
		buf_left[idx * chroma_width] = buf_right[idx * chroma_width] = 255;
	}
}

void main(int argc, char **argv)
{
	cout << "----------Test-------------" << endl;
	unsigned int width = 352, height = 288;
	unsigned int luma_size = width * height;
	unsigned int chroma_size = luma_size / 2;
	unsigned int frame_size = luma_size + chroma_size;
    char *buf = new char[frame_size];
	unsigned int idx = 0;
	
	string in_path = "F:\\rkvenc_verify\\input_yuv\\Bus_352x288_25.yuv";

	istream *ifs = new ifstream(in_path.c_str(), ios::binary | ios::in);
	ifs->read(buf, frame_size);

	YuvInfo yuv_info;
	RectangleInfo rec_info;
	yuv_info.buf = buf;
	yuv_info.width = width;
	yuv_info.height = height;
	rec_info.left = 20;
	rec_info.top = 10;
	rec_info.right = 100;
	rec_info.bottom = 50;

	draw_red_rectangle(&yuv_info, &rec_info);

	string out_path = "F:\\rkvenc_verify\\input_yuv\\Red_352x288_25.yuv";
	ofstream ofs;
	ofs.open(out_path.c_str(), ios::binary | ios::out);
	ofs.write(buf, frame_size);

	cout << "----------End!-------------" << endl;

	if (ifs && ifs != &cin)
		delete ifs;
	ofs.close();
	delete [] buf;

	string str;
	cin >> str;
	return;
}
