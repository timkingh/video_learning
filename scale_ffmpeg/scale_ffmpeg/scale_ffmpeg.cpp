#include <iostream>
#include <fstream>
#include <string>
#include "stdint.h"

using namespace std;

int main()
{
    cout << "Start!" << endl;
    string in_path = "F:\\yuv\\yyq.yuv";
    string out_path = "F:\\yuv\\yyq_out.yuv";

    cout << "input: " << in_path << endl
         << "output: " << out_path << endl;
    istream *ifs = new ifstream(in_path.c_str(), ios::binary | ios::in);
    ofstream ofs;
    ofs.open(out_path.c_str(), ios::binary | ios::out);

    uint32_t frames = 10, frame_cnt = 0;
    uint32_t width = 1280, height = 720;
    uint32_t frame_size = width * height * 3 / 2;
    char *buf = new char[frame_size];

    while (frame_cnt++ < frames) {
        ifs->read(buf, frame_size);
        ofs.write(buf, frame_size);
    }

    cout << "End!" << endl;

    if (ifs && ifs != &cin)
        delete ifs;
    ofs.close();
    if (buf)
        delete [] buf;

    return 0;
}
