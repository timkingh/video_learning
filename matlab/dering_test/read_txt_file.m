clear all
clc

width = 1280;
height = 720;

fid_dering0 = fopen('jpg_dec_street_out40_hisi_r15_dering0.txt', 'r');
fid_dering1 = fopen('jpg_dec_street_out40_hisi_r15_dering1.txt', 'r');
fid_var = fopen('out_var_1.txt', 'r');
fid_out = fopen('dct_cmp_out_rk_15000.txt', 'w');

qtable = [
     3   2   2   3   5   8  10  12
     2   2   3   4   5  12  12  11
     3   3   3   5   8  11  14  11
     3   3   4   6  10  17  16  12
     4   4   7  11  14  22  21  15
     5   7  11  13  16  21  23  18
    10  13  16  17  21  24  24  20
    14  18  19  20  22  20  21  20
];

blk_num = 0;

for row = 1:16:height
    for col = 1:16:width
        for idx = 0:3
            pos_x = col - 1 + rem(idx, 2) * 8;
            pos_y = row - 1 + floor(idx / 2) * 8;
            coef0 = fscanf(fid_dering0, '%d', [8, 8]);
            coef1 = fscanf(fid_dering1, '%d', [8, 8]);
            blk8_var = fscanf(fid_var, '%d', 1);
            for m = 1:8
                for n = 1:8
                    if coef0(m, n) ~= coef1(m, n)
                        rem_v = rem(abs(coef0(m, n) - coef1(m, n)), qtable(m, n));
                        if rem_v ~= 0
                            fprintf(fid_out, "pos(%d, %d) blk8_pos(%d, %d)\n", ...
                                    pos_x, pos_y, m, n);
                            blk_num = blk_num + 1;   
                        end
                    end
                end
            end
%             if blk8_var > 15000 % isequal(coef0, coef1) ~= 1
%                 fprintf(fid_out, "frame=0, cu_x=%d, cu_y=%d, cu_size=8, mv_x=0, mv_y=-2\n", ...
%                         pos_x, pos_y);
%                 blk_num = blk_num + 1;
%             end
        end
    end
end


fclose(fid_dering0);
fclose(fid_dering1);
fclose(fid_var);
fclose(fid_out);

fprintf("blk_num %d\n", blk_num);