clear all
clc

T = dctmtx(8);

width = 1280;
height = 720;

fid = fopen("F:\nfs\street_720p_out_42.yuv", "r");

y_mtx = fread(fid, [width, height]);

out_mtx = y_mtx';

fclose(fid);

blk = out_mtx(1:8, 465:472)
 
coef = T * (blk - 128) * T';
codf_r = round(coef);
 
 