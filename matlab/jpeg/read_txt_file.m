clear all
clc

width = 1280;
height = 720;

fid_dering0 = fopen('./input_file/jpg_dec_coef_test_r67_hisi_r59_q4_dering0.txt', 'r');
fid_dering1 = fopen('./input_file/jpg_dec_coef_test_r67_hisi_r59_q4_dering1.txt', 'r');
fid_var = fopen('./input_file/var_coef_test_r67.txt', 'r');
fid_madi = fopen('./input_file/madi_coef_test_r67.txt', 'r');
fid_out = fopen('dct_cmp_out.txt', 'w');

read_mtx = fscanf(fid_var, '%d', [width / 8, height / 8]);
pic_var = read_mtx';

read_mtx = fscanf(fid_madi, '%d', [width / 8, height / 8]);
pic_madi = read_mtx';

blk_num = 0;
out_flag = 0;

for row = 1:16:height
    for col = 1:16:width
        for idx = 0:3
            pos_x = col - 1 + rem(idx, 2) * 8;
            pos_y = row - 1 + floor(idx / 2) * 8;
            coef0 = fscanf(fid_dering0, '%d', [8, 8]);
            coef1 = fscanf(fid_dering1, '%d', [8, 8]);
            
            if coef0(8, 8) == 4 && coef1(8, 8) == 0
                out_flag = 1;
            else
                out_flag = 0;
            end
            
            if out_flag == 1
                blk8_x = pos_x / 8 + 1;
                blk8_y = pos_y / 8 + 1;
                fprintf(fid_out, "pos(%d, %d) DC %4d c0=%d c1=%d diff=%d var=%d madi=%d\n\n", ...
                        pos_x, pos_y, coef0(1, 1), coef0(8, 8), coef1(8, 8), ...
                        abs(coef0(8, 8) - coef1(8, 8)), ...
                        pic_var(blk8_y, blk8_x), pic_madi(blk8_y, blk8_x));
%                 fprintf(fid_out, "%4d %4d %4d %4d %4d %4d %4d %4d\n\n", coef0);
            end
        end
    end
end


fclose('all');
fprintf("blk_num %d\n", blk_num);