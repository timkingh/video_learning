clear all
clc

width = 1920;
height = 1080;

file_path = "D:\code\video_learning\matlab\dering_test\input_file\";
file_name = file_path + "jpg_dec_street_1080p_part_hisi_r73_qf56_dering0.txt";
[coef0, pos0] = open_jpeg_coef_file(file_name, width, height);

file_name = file_path + "jpg_dec_street_1080p_part_hisi_r73_qf56_dering1.txt";
[coef1, pos1] = open_jpeg_coef_file(file_name, width, height);

blk8_num = width / 8 * height / 8;
blk8_dr0 = zeros(8, 8);
blk8_dr1 = zeros(8, 8);

qtable_delta = [
     0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0
     0 1 1 1 0 0 0 0
     0 0 1 1 0 0 0 0
     0 0 1 0 0 0 0 0
     0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0
];

for iter = 1:blk8_num
    blk8_dr0 = coef0(:, :, iter);
    blk8_dr1 = coef1(:, :, iter);
    pos_x = pos0(iter, 1);
    pos_y = pos0(iter, 2);
    
    if isequal(qtable_delta, abs(blk8_dr0 - blk8_dr1))
        fprintf("pos(%d, %d)\n", pos_x, pos_y);
    end
end
