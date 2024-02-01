clear all
clc

width = 1920;
height = 1080;
blk8_num = 80;
min_v = 0;
max_v = 255;
y_mtx = zeros(height, width);
uv_mtx = zeros(height / 2, width);

[fid, msg] = fopen("F:\nfs\street_1080p_part.yuv", "r");
if fid == -1
    disp(msg);
    return;
end

[fid_out, msg] = fopen("D:\code\video_learning\matlab\dct_spectrum\output\coef_test_r80.yuv", "w");
if fid_out == -1
    disp(msg);
    return;
end

y_mtx = fread(fid, [width, height]);
uv_mtx = fread(fid, [width, height / 2]);
out_y = y_mtx';

blk8_cnt = 1;
for k = 1:16:height 
    for j = 1:16:width
        for idx = 0:3
            pos_x = j - 1 + rem(idx, 2) * 8;
            pos_y = k - 1 + floor(idx / 2) * 8;   
            out_y(pos_y + 1 + row, pos_x + 1 + col) = floor(out_y(pos_y + 1 + row, pos_x + 1 + col) / 2);
        end
    end
end


count = fwrite(fid_out, out_y', 'uint8');
count = count + fwrite(fid_out, uv_mtx', 'uint8');

fprintf("fwrite %d bytes\n", count);
fclose(fid);
fclose(fid_out);