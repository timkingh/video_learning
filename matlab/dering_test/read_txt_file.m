clear all
clc
tic

width = 1280;
height = 720;

fid_dering0 = fopen('jpg_dec_random_720p_hisi_r27_qt10_dering0.txt', 'r');
fid_dering1 = fopen('jpg_dec_random_720p_hisi_r27_qt10_dering1.txt', 'r');
fid_var = fopen('out_var_street_720p.txt', 'r');
fid_out = fopen('dec_cmp_out_rk_1.txt', 'w');
fid_out_0 = fopen('dering0.txt', 'w');
fid_out_1 = fopen('dering1.txt', 'w');

qtable = ones(8, 8) * 10;


blk_num = 0;
out_mtx = zeros(8, 8);
min_idx = 16;
max_multi = 0;

for row = 1:16:height
    for col = 1:16:width
        for idx = 0:3
            pos_x = col - 1 + rem(idx, 2) * 8;
            pos_y = row - 1 + floor(idx / 2) * 8;
            coef0 = fscanf(fid_dering0, '%d', [8, 8]);
            coef1 = fscanf(fid_dering1, '%d', [8, 8]);
            dir = fscanf(fid_var, 'pos(%d, %d) dir %d var %d\n', [4, 1]);
            blk_num = 0; 
            fprintf(fid_out, "pos(%d, %d)\n", pos_x, pos_y);
            for n = 1:8
                for m = 1:8
                    if coef0(m, n) ~= coef1(m, n)
                        out_mtx(m, n) = abs(coef0(m, n) - coef1(m, n)) / qtable(n, m);
                    end
                    fprintf(fid_out, "%4d ", out_mtx(m, n));
                end
                fprintf(fid_out, "\n\n");
            end
            fprintf(fid_out, "\n\n");
            
            if max(max(out_mtx)) > max_multi
                max_multi = max(max(out_mtx));
            end
        end
    end
end

fclose(fid_dering0);
fclose(fid_dering1);
fclose(fid_var);
fclose(fid_out);
fclose(fid_out_0);
fclose(fid_out_1);


fprintf("max_multi %d\n", max_multi);
toc