clear all
clc

width = 1920;
height = 1088;

zsantoraster = [
        0,  1,  8, 16,  9,  2,   3, 10, ...
        17, 24, 32, 25, 18, 11,  4,  5, ...
        12, 19, 26, 33, 40, 48, 41, 34, ...
        27, 20, 13,  6,  7, 14, 21, 28, ...
        35, 42, 49, 56, 57, 50, 43, 36, ...
        29, 22, 15, 23, 30, 37, 44, 51, ...
        58, 59, 52, 45, 38, 31, 39, 46, ...
        53, 60, 61, 54, 47, 55, 62, 63
];

fid_dering0 = fopen('./input_file/concat_b_1080p_jpege_r52_12frames.txt', 'r');
fid_dering1 = fopen('./input_file/jpg_dec_coef_test_r67_hisi_r59_q4_dering1.txt', 'r');
fid_var = fopen('./input_file/var_coef_test_r67.txt', 'r');
fid_madi = fopen('./input_file/madi_coef_test_r67.txt', 'r');
fid_out = fopen('dct_coef_out_matlab_r76.txt', 'w');

blk8_num = width * height / 64 * 12;
out_flag = 0;

for idx = 1:blk8_num
    info = fscanf(fid_dering0, 'frame %d pos(%d, %d) dir %d var %d\n');
    frm_num = info(1, 1);
    pos_x = info(2, 1);
    pos_y = info(3, 1);
    dir = info(4, 1);
    var = info(5, 1);
    
    coef0 = fscanf(fid_dering0, '%d', [8, 8]);
    fscanf(fid_dering0, '\n');
    coef_res = reshape(coef0, 64, 1);
    
    non_zero_cnt = 0;
    for m = 37:64
        raster_idx = zsantoraster(1, m) + 1;
        if coef_res(raster_idx, 1) ~= 0
            non_zero_cnt = non_zero_cnt + 1;
        end
    end

    if var < 128 && non_zero_cnt > 4
        fprintf(fid_out, "frame %d pos(%d, %d) non_zero_cnt %d\n\n", ...
                frm_num, pos_x, pos_y, non_zero_cnt);
    end
end

fclose('all');
fprintf("blk8_num %d\n", blk8_num);