clear all
clc

width = 1280;
height = 720;
blk8_num = 80;
blk8_sets = zeros(8, 8, blk8_num);
min_v = 0;
max_v = 255;
y_mtx = zeros(height, width);
uv_mtx = ones(height / 2, width / 2) .* 128;

blk8_1_2_min = [
     0     0     0     0     0     0     0     0
   240   240   240   240   240   240   240   240
   243   243   243   243   243   243   243   243
   246   246   246   246   246   246   246   246
   249   249   249   249   249   249   249   249  
   252   252   252   252   252   252   252   252
   255   255   255   255   255   255   255   255    
   243   243   243   243   243   243   243   243 
];

blk8_1_2_max = [
   240   240   240   240   240   240   240   240
     0     0     0     0     0     0     0     0
   243   243   243   243   243   243   243   243
   246   246   246   246   246   246   246   246
   249   249   249   249   249   249   249   249  
   252   252   252   252   252   252   252   252
   255   255   255   255   255   255   255   255    
   243   243   243   243   243   243   243   243    
];

blk8_1_3_min = [
   240   240   240   240   240   240   240   240
   243   243   243   243   243   243   243   243
     0     0     0     0     0     0     0     0
   246   246   246   246   246   246   246   246
   249   249   249   249   249   249   249   249  
   252   252   252   252   252   252   252   252
   255   255   255   255   255   255   255   255    
   243   243   243   243   243   243   243   243 
];

blk8_1_3_max = [     
   240   240   240   240   240   240   240   240
   243   243   243   243   243   243   243   243
   246   246   246   246   246   246   246   246
     0     0     0     0     0     0     0     0
   249   249   249   249   249   249   249   249  
   252   252   252   252   252   252   252   252
   255   255   255   255   255   255   255   255    
   243   243   243   243   243   243   243   243   
];

blk8_1_4_min = [
   240   240   240   240   240   240   240   240
   243   243   243   243   243   243   243   243
   246   246   246   246   246   246   246   246
   249   249   249   249   249   249   249   249  
     0     0     0     0     0     0     0     0   
   252   252   252   252   252   252   252   252
   255   255   255   255   255   255   255   255    
   243   243   243   243   243   243   243   243 
];

blk8_1_4_max = [
   240   240   240   240   240   240   240   240
   243   243   243   243   243   243   243   243
   246   246   246   246   246   246   246   246
   249   249   249   249   249   249   249   249  
   252   252   252   252   252   252   252   252
     0     0     0     0     0     0     0     0   
   255   255   255   255   255   255   255   255    
   243   243   243   243   243   243   243   243 
];

blk8_2_1_min = [
   240   240   240   240   240   240   240   240
   243   243   243   243   243   243   243   243
   246   246   246   246   246   246   246   246
   249   249   249   249   249   249   249   249  
   252   252   252   252   252   252   252   252
   255   255   255   255   255   255   255   255
     0     0     0     0     0     0     0     0   
   243   243   243   243   243   243   243   243 
];

blk8_2_1_max = [
   240   240   240   240   240   240   240   240
   243   243   243   243   243   243   243   243
   246   246   246   246   246   246   246   246
   249   249   249   249   249   249   249   249  
   252   252   252   252   252   252   252   252
   255   255   255   255   255   255   255   255      
   243   243   243   243   243   243   243   243 
     0     0     0     0     0     0     0     0    
];

blk8_2_2_min = [
    0     0     0     0   255   255   255   255 
    0     0     0   255   255   255     0   255 
    0     0     0     0     0   255   255   255 
    0   255     0     0     0   255   255   255 
    0     0     0     0     0   255     0   255 
  255   255     0   255     0     0     0     0 
  255   255     0     0     0     0     0     0 
  255   255   255   255     0   255     0     0 
];

blk8_2_2_max = [
  255   255   255     0   255     0     0     0 
  255   255     0   255     0     0     0     0 
  255   255   255     0   255     0   255     0 
  255     0     0   255   255     0     0     0 
    0   255     0   255   255   255   255     0 
    0     0     0   255   255     0   255   255 
    0     0     0     0   255   255   255   255 
    0   255     0     0     0   255   255   255 
];

blk8_2_3_min = [
    0     0     0   255   255   255     0     0 
    0     0   255     0   255   255   255     0 
    0   255   255   255   255     0     0     0 
  255   255     0   255   255     0   255     0 
  255     0     0     0     0   255     0   255 
  255   255   255     0   255     0     0   255 
  255   255   255     0     0     0   255   255 
  255   255     0     0     0     0     0   255 
];

blk8_2_3_max = [
  255   255     0     0     0     0   255   255 
  255   255     0     0     0   255   255   255 
  255     0     0     0     0     0     0   255 
  255     0     0   255   255     0   255   255 
    0   255   255     0   255   255     0   255 
    0     0     0   255     0     0   255     0 
    0   255   255   255   255   255   255     0 
    0     0   255   255   255   255   255   255 
];

blk8_3_1_min = [
  255     0     0     0     0     0     0     0 
    0     0     0     0     0     0     0   255 
    0     0   255     0   255   255   255   255 
  255   255   255   255   255   255   255   255 
  255   255   255   255   255   255   255     0 
    0   255   255   255   255   255     0   255 
  255     0     0   255     0     0     0   255 
    0     0   255   255     0     0     0     0     
];

blk8_3_1_max = [
  255   255   255   255   255   255     0   255 
    0     0   255   255     0   255   255     0 
    0     0   255     0     0     0     0     0 
    0     0     0     0     0     0     0     0 
    0   255     0   255     0   255     0     0 
    0   255     0   255     0   255     0   255 
    0   255   255   255     0   255   255     0 
  255   255   255   255   255   255   255   255     
];

blk8_3_2_min = [
    0     0     0     0   255   255   255   255 
    0     0     0     0     0     0   255   255 
    0   255     0   255   255   255   255     0 
  255     0   255   255     0     0     0     0 
  255   255   255   255   255     0     0     0 
  255   255   255   255     0     0     0   255 
    0     0   255   255     0   255   255   255 
    0     0     0     0   255     0     0   255     
];

blk8_3_2_max = [
  255   255   255     0     0     0     0     0 
  255   255     0   255     0     0   255     0 
    0     0     0     0     0     0   255   255 
    0     0   255   255     0   255   255   255 
    0   255     0     0     0   255   255   255 
  255     0     0   255     0   255   255   255 
  255   255   255   255     0   255     0   255 
  255   255   255     0     0     0     0     0 
];

blk8_4_1_min = [
    0     0     0     0     0     0     0     0 
  255     0     0   255     0     0   255     0 
  255   255   255   255   255   255   255   255 
    0     0     0     0   255   255     0   255 
    0     0     0     0     0     0     0     0 
    0     0     0     0     0     0     0     0 
    0   255   255   255   255   255   255   255 
  255   255   255     0   255   255   255   255     
];

blk8_4_1_max = [
    0     0   255   255   255   255   255   255 
    0   255     0     0     0   255     0     0 
    0     0     0     0     0     0   255     0 
    0     0   255   255     0     0     0     0 
  255   255     0     0   255     0   255   255 
  255   255   255   255     0   255   255   255 
  255   255   255   255   255   255   255     0 
    0     0     0   255     0     0     0     0     
];

blk8_sets(:, :, 1) = blk8_1_2_min;
blk8_sets(:, :, 2) = blk8_1_2_max;
blk8_sets(:, :, 3) = blk8_1_3_min;
blk8_sets(:, :, 4) = blk8_1_3_max;
blk8_sets(:, :, 5) = blk8_1_4_min;
blk8_sets(:, :, 6) = blk8_1_4_max;
blk8_sets(:, :, 7) = blk8_2_1_min;
blk8_sets(:, :, 8) = blk8_2_1_max;
blk8_sets(:, :, 9) = blk8_2_2_min;
blk8_sets(:, :, 10) = blk8_2_2_max;
blk8_sets(:, :, 11) = blk8_2_3_min;
blk8_sets(:, :, 12) = blk8_2_3_max;
blk8_sets(:, :, 13) = blk8_3_1_min;
blk8_sets(:, :, 14) = blk8_3_1_max;
blk8_sets(:, :, 15) = blk8_3_2_min;
blk8_sets(:, :, 16) = blk8_3_2_max;
blk8_sets(:, :, 17) = blk8_4_1_min;
blk8_sets(:, :, 18) = blk8_4_1_max;
for k = 19:blk8_num
    blk8_sets(:, :, k) = randi([0, 255], 8, 8);
end

[fid_out, msg] = fopen("D:\code\video_learning\matlab\dct_spectrum\output\coef_test_r65.yuv", "w");
if fid_out == -1
    disp(msg);
    return;
end

for k = 1:16:height
    for j = 1:16:width
        for idx = 0:3
            pos_x = j - 1 + rem(idx, 2) * 8;
            pos_y = k - 1 + floor(idx / 2) * 8;
            rand_v = randi([0, 255], 1, 15);
            for row = 0:7
                for col = 0:7
                    y_mtx(pos_y + 1 + row, pos_x + 1 + col) = rand_v(1, row - col + 8);
%                     y_mtx(pos_y + 1 + row, pos_x + 1 + col) = rand_v(1, row + col + 1);
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