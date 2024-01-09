clear all
clc

load("blk8.mat");

width = 1280;
height = 720;
blk8_num = 80;
min_v = 0;
max_v = 255;
y_mtx = zeros(height, width);
uv_mtx = ones(height / 2, width / 2) .* 128;


[fid_out, msg] = fopen("D:\code\video_learning\matlab\dct_spectrum\output\coef_test_r68.yuv", "w");
if fid_out == -1
    disp(msg);
    return;
end

blk8_cnt = 1;
for k = 1:16:height
    for j = 1:16:width
        for idx = 0:3
            pos_x = j - 1 + rem(idx, 2) * 8;
            pos_y = k - 1 + floor(idx / 2) * 8;       
            if blk8_cnt < 1000
                y_mtx(pos_y + 1:pos_y + 8, pos_x + 1:pos_x + 8) = min_blk8(:, :, blk8_cnt);
                blk8_cnt = blk8_cnt + 1;
            end
        end
    end
end


count = fwrite(fid_out, y_mtx', 'uint8');
count = count + fwrite(fid_out, uv_mtx', 'uint8');
count = count + fwrite(fid_out, uv_mtx', 'uint8');

fprintf("fwrite %d bytes\n", count);

fclose(fid_out);