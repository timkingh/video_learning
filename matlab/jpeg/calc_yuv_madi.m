clear all
clc

width = 1280;
height = 720;

fid = fopen("F:\nfs\street_720p.yuv", "r");

y_mtx = fread(fid, [width, height]);

out_mtx = y_mtx';

fclose(fid);

blk = out_mtx(105:112, 769:776);
 
 
sum = 0;
for m = 1:8
    for n = 1:8
         sum = sum + (209 - blk(m, n)) * (209 - blk(m, n));
    end
end
 