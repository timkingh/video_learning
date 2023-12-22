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
coef_num = 64;
blk8_sets = zeros(8, 8, blk8_num * coef_num);
blk8_ready_flag = zeros(blk8_num, coef_num);
blk8_total_cnt = 0;

T = dctmtx(8);
min_v = 0;
max_v = 255;
value = [ min_v, max_v ];
iter_num = 100000000;

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
    
    for row = 1:8
        for col = 1:8
            blk8_idx = ceil(coef(row, col) / 10);
            coef_index = (row - 1) * 8 + (col - 1);
            if blk8_ready_flag(blk8_idx + 128, coef_index + 1) == 0
                blk8_total_cnt = blk8_total_cnt + 1;
                blk8_sets(:, :, blk8_total_cnt) = blk8;
                blk8_ready_flag(blk8_idx + 128, coef_index + 1) = 1;                
                break;
            end
        end
    end
    
    for hh = 1:8
        if m == running_rate(1, hh)
            fprintf("running %f blk8_total_cnt %d\n", m / iter_num, blk8_total_cnt);
            break;
        end
    end
end


for n = 1:coef_num
    not_ready_num = 0;
    for m =1:blk8_num
        if blk8_ready_flag(m, n) == 0
            not_ready_num = not_ready_num + 1;
        end
    end
    
    fprintf("coef_idx %d not_ready_num %d\n", n, not_ready_num);
end


[fid_out_yuv, msg] = fopen("D:\code\video_learning\matlab\dering_test\coef_test_r43.yuv", "w");
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
        blk8_idx = rem(blk8_idx, blk8_total_cnt);
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
        blk8_idx = rem(blk8_idx, blk8_total_cnt);
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