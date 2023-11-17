clear all
clc

width = 1280;
height = 720;

fid = fopen("F:\nfs\street_720p_out_4.yuv", "r");

y_mtx = fread(fid, [width, height]);

out_mtx = y_mtx';

fclose(fid);