#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void main(int argc, char **argv)
{
	cout << "----------Test-------------" << endl;
	unsigned int luma_size = 352 * 288;
	unsigned int chroma_size = luma_size / 2;
	unsigned int frame_size = luma_size + chroma_size;
    char *buf = new char[frame_size];
	unsigned int idx = 0;
	
	string in_path = "F:\\rkvenc_verify\\input_yuv\\Bus_352x288_25.yuv";

	istream *ifs = new ifstream(in_path.c_str(), ios::binary | ios::in);
	ifs->read(buf, frame_size);
	for (idx = 0; idx < luma_size; idx++) {
		buf[idx] = 76;
	}

	for (idx = luma_size; idx < luma_size + chroma_size / 2; idx++) {
		buf[idx] = 0;
	}
	
	for (idx = luma_size + chroma_size / 2; idx < frame_size; idx++) {
		buf[idx] = 157;
	}

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
