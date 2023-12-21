clear all
%clc
tic

[fid_out, msg] = fopen("D:\code\video_learning\matlab\dering_test\log_r0.txt", "w");
if fid_out == -1
    disp(msg);
    return;
end

width = 1280;
height = 720;
blk8_num = 256;
blk8_sets = zeros(8, 8, blk8_num);
blk8_ready_flag = zeros(256, 1);

T = dctmtx(8);
min_v = 0;
max_v = 255;
value = [ min_v, max_v ];
iter_num = 10000000;

running_rate = zeros(1, 8);
for m = 1:8
    running_rate(1, m) = iter_num * m / 8;
end

for m = 1:iter_num
    blk8 = [ value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
            ];

    coef = T * (blk8 - 128) * T';
    coef = round(coef);
    blk8_idx = ceil(coef(8, 8) / 10);
    
    if blk8_ready_flag(blk8_idx + 128, 1) == 0
        blk8_sets(:, :, blk8_idx + 128) = blk8;
        blk8_ready_flag(blk8_idx + 128, 1) = 1;
    end
    
    for hh = 1:8
        if m == running_rate(1, hh)
            fprintf("running %f\n", m / iter_num);
            break;
        end
    end
end

for m =1:blk8_num
    if blk8_ready_flag(m, 1) == 0
        fprintf("blk8 %d not ready\n", m);
    end
end

[fid_out_yuv, msg] = fopen("D:\code\video_learning\matlab\dering_test\coef_test_r3_v2.yuv", "w");
if fid_out_yuv == -1
    disp(msg);
    return;
end

y_mtx = zeros(height, width);
uv_mtx = zeros(height / 2, width / 2);

blk8_idx = -1;
for k = 1:8:height
    for j = 1:8:width
        blk8_idx = blk8_idx + 1;
        blk8_idx = rem(blk8_idx, 256);
        if blk8_idx == 0
            blk8_idx = 1;
        end
        y_mtx(k:k+7, j:j+7) = blk8_sets(:, :, blk8_idx);
    end
end

blk8_idx = -1;
for k = 1:8:height/2
    for j = 1:8:width/2
        blk8_idx = blk8_idx + 1;
        blk8_idx = rem(blk8_idx, 256);
        if blk8_idx == 0
            blk8_idx = 1;
        end
        uv_mtx(k:k+7, j:j+7) = blk8_sets(:, :, blk8_idx);
    end
end

count = fwrite(fid_out_yuv, y_mtx', 'uint8');
count = count + fwrite(fid_out_yuv, uv_mtx', 'uint8');
count = count + fwrite(fid_out_yuv, uv_mtx', 'uint8');

fprintf("fwrite %d bytes\n", count);

fclose(fid_out);
fclose(fid_out_yuv);

fprintf("finished!\n");
toc