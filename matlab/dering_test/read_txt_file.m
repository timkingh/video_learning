clear all
clc

width = 1280;
height = 720;

fid_dering0 = fopen('jpg_dec_street_out27_hisi_r15_dering0.txt', 'r');
fid_dering1 = fopen('jpg_dec_street_out27_hisi_r15_dering1.txt', 'r');
fid_out = fopen('dct_cmp_out.txt', 'w');

blk_num = 0;

for row = 1:16:height
    for col = 1:16:width
        for idx = 0:3
            pos_x = col - 1 + rem(idx, 2) * 8;
            pos_y = row - 1 + floor(idx / 2) * 8;
            coef0 = fscanf(fid_dering0, '%d', [8, 8]);
            coef1 = fscanf(fid_dering1, '%d', [8, 8]);
            if isequal(coef0, coef1) ~= 1
                fprintf(fid_out, "frame=0, cu_x=%d, cu_y=%d, cu_size=8, mv_x=0, mv_y=-2\n", ...
                        pos_x / 8, pos_y / 8);
                blk_num = blk_num + 1;
            end
        end
    end
end


fclose(fid_dering0);
fclose(fid_dering1);
fclose(fid_out);

fprintf("blk_num %d\n", blk_num);