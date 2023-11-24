clear all
clc

T = dctmtx(8);

width = 1280;
height = 720;

fid = fopen("F:\nfs\random_720p.yuv", "r");

y_mtx = fread(fid, [width, height]);

out_mtx = y_mtx';

fclose(fid);

 blk = out_mtx(1:8, 1:8);
 
 T * (blk - 128) * T'