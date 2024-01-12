clear all
clc

load("blk8_1.mat");

width = 1280;
height = 720;
blk8_num = 80;
min_v = 0;
max_v = 255;
y_mtx = zeros(height, width);
uv_mtx = ones(height / 2, width / 2) .* 128;


[fid_out, msg] = fopen("D:\code\video_learning\matlab\dct_spectrum\output\coef_test_r72.yuv", "w");
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
            rand_v = randi([0, 255], 1, 15);
            sort_v = sort(rand_v);
            for row = 0:7
                for col = 0:7
                    y_mtx(pos_y + 1 + row, pos_x + 1 + col) = sort_v(1, row + col + 1);
                end
            end
        end
    end
end


count = fwrite(fid_out, y_mtx', 'uint8');
count = count + fwrite(fid_out, uv_mtx', 'uint8');
count = count + fwrite(fid_out, uv_mtx', 'uint8');

fprintf("fwrite %d bytes\n", count);

fclose(fid_out);