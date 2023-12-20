clear all
clc
tic

width = 1280;
height = 720;

[fid_dering0, msg] = fopen('.\log\jpg_dec_random_720p_hisi_r30_qt10_dering0.txt', 'r');
if fid_dering0 == -1
    disp(msg);
    return;
end

fid_dering1 = fopen('.\log\jpg_dec_random_720p_hisi_r30_qt10_dering1.txt', 'r');
if fid_dering1 == -1
    disp(msg);
    return;
end

fid_var = fopen('.\log\out_var_street_720p.txt', 'r');
fid_out = fopen('.\log\jpg_dec_random_720p_hisi_r30_qt10_dering_cmp_1.txt', 'w');
fid_out_0 = fopen('.\log\dering0.txt', 'w');
fid_out_1 = fopen('.\log\dering1.txt', 'w');

qtable = ones(8, 8) * 10;
min_idx = 16;
max_multi = 0;
max_coef = -2048;
min_coef = 2048;
pos_mtx = zeros(2, 2);

for row = 1:16:height
    for col = 1:16:width
        for idx = 0:3
            pos_x = col - 1 + rem(idx, 2) * 8;
            pos_y = row - 1 + floor(idx / 2) * 8;
            coef0 = fscanf(fid_dering0, '%d', [8, 8]);
            coef1 = fscanf(fid_dering1, '%d', [8, 8]);
            dir = fscanf(fid_var, 'pos(%d, %d) dir %d var %d\n', [4, 1]);
            out_mtx = zeros(8, 8);
            fprintf(fid_out, "pos(%d, %d)\n", pos_x, pos_y);

            for n = 1:8 % row
                for m = 1:8 % col
                    if coef0(m, n) ~= coef1(m, n)
                        out_mtx(m, n) = abs(coef0(m, n) - coef1(m, n)) / qtable(n, m);
                    end  
           
                    if (n == 1 && (m >= 2 && m <= 4)) || ...
                        (n == 2 && (m >= 1 && m <= 3)) || ...
                        (n == 3 && (m >= 1 && m <= 2)) || ...
                        (n == 4 && m == 1)
                        if coef0(m, n) > max_coef
                            max_coef = coef0(m, n);
                            pos_mtx(1, 1) = pos_x;
                            pos_mtx(1, 2) = pos_y;
                        end
                    
                        if coef0(m, n) < min_coef
                            min_coef = coef0(m, n);
                            pos_mtx(2, 1) = pos_x;
                            pos_mtx(2, 2) = pos_y;
                        end
                    end
                end
            end         
        end
    end
end

fprintf(fid_out, "max %d pos(%d, %d) min %d pos(%d, %d)\n", ...
        max_coef, pos_mtx(1, 1), pos_mtx(1, 2), ...
        min_coef, pos_mtx(2, 1), pos_mtx(2, 2));
   
   
fclose(fid_dering0);
fclose(fid_dering1);
fclose(fid_var);
fclose(fid_out);
fclose(fid_out_0);
fclose(fid_out_1);


fprintf("finished\n");
toc