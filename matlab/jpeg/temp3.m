clear all
clc

width = 1280;
height = 720;
min_v = 0;
max_v = 255;
y_mtx = zeros(height, width);
uv_mtx = zeros(height / 2, width);

blk8 = [   min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
           max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
           min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
           max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
           min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
           max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
           min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
           max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
        ];

fid_out = fopen("F:\nfs\coef_test_roi_1.yuv", "w");

for k = 1:8:height
    for j = 1:8:width
        y_mtx(k:k+7, j:j+7) = blk8;
    end
end

for k = 1:8:height/2
    for j = 1:8:width
        uv_mtx(k:k+7, j:j+7) = blk8;
    end
end

count = fwrite(fid_out, y_mtx', 'uint8');
count = count + fwrite(fid_out, uv_mtx', 'uint8');

fprintf("fwrite %d bytes\n", count);

fclose(fid_out);