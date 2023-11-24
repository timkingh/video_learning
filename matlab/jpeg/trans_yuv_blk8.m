clear all
clc

width = 1280;
height = 720;

fid = fopen("F:\nfs\random_720p.yuv", "r");

y_mtx = fread(fid, [width, height]);

out_mtx = y_mtx';

fclose(fid);

blk8_cnt = 0;
out_yuv = ones(height, width);

for m = 1:8:height
    for n =1:8:width
        out_yuv(m:m+7, n:n+7) = get_blk8_from_yuv(out_mtx, width, height, blk8_cnt);
        blk8_cnt = blk8_cnt + 1;
    end
end

fid_yuv = fopen("F:\nfs\random_720p_gray.yuv", "w");

count = fwrite(fid_yuv, out_yuv', 'uint8');
fprintf("fwrite %d bytes\n", count);

fclose(fid_yuv);
