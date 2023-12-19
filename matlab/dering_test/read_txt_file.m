clear all
clc
tic

width = 1280;
height = 720;

[fid_dering0, msg] = fopen('.\log\jpg_dec_street_720p_hisi_r30_qt10_dering0.txt', 'r');
if fid_dering0 == -1
    disp(msg);
    return;
end

fid_dering1 = fopen('.\log\jpg_dec_street_720p_hisi_r30_qt10_dering1.txt', 'r');
if fid_dering1 == -1
    disp(msg);
    return;
end

fid_var = fopen('out_var_street_720p.txt', 'r');
fid_out = fopen('dec_cmp_out_rk_1.txt', 'w');
fid_out_0 = fopen('dering0.txt', 'w');
fid_out_1 = fopen('dering1.txt', 'w');

qtable = ones(8, 8) * 1;


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

            for n = 1:8 % row
                for m = 1:8 % col
                    if coef0(m, n) ~= coef1(m, n)
                        out_mtx(m, n) = abs(coef0(m, n) - coef1(m, n)) / qtable(n, m);
                        if out_mtx(m, n) > 0
                            fprintf(fid_out, "pos(%d, %d) (%d, %d) %4d %4d diff=%d\n", ...
                                    pos_x, pos_y, n, m, ...
                                    coef0(m, n), coef1(m, n), out_mtx(m, n));
                        end
                    end  
                    
                    if (n == 1 && (m >= 1 && m <= 6)) || ...
                       (n == 2 && (m >= 1 && m <= 5)) || ...
                       (n == 3 && (m >= 1 && m <= 3)) || ...
                       (n == 4 && m == 1)
                        if coef0(m, n) ~= coef1(m, n)
                            fprintf("pos(%d, %d) (%d, %d) %4d %4d diff=%d\n", ...
                                    pos_x, pos_y, n, m, ...
                                    coef0(m, n), coef1(m, n), out_mtx(m, n));
                        end
                    end
                end
            end
            
%             if max(max(out_mtx)) > max_multi
%                 max_multi = max(max(out_mtx));
%             end
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