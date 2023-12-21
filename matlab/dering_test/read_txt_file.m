clear all
clc
tic

width = 1280;
height = 720;

[fid_dering0, msg] = fopen('.\log\jpg_dec_random_720p_hisi_r27_qt10_dering0.txt', 'r');
if fid_dering0 == -1
    disp(msg);
    return;
end

fid_dering1 = fopen('.\log\jpg_dec_random_720p_hisi_r27_qt10_dering1.txt', 'r');
if fid_dering1 == -1
    disp(msg);
    return;
end

fid_var = fopen('.\log\out_var_street_720p.txt', 'r');
fid_out = fopen('.\log\jpg_dec_coef_test_r1_hisi_r36_qt10_dering_mr2.txt', 'w');
fid_out_0 = fopen('.\log\x1_r6.txt', 'w');
fid_out_1 = fopen('.\log\y1_r6.txt', 'w');

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
            flag_same_coef = 1;
            flag_diff_coef = 0;
            
            for n = 1:8 % row
                for m = 1:8 % col
                    if (n == 8 && m >= 7 && m <= 8) || ...
                       (n == 7 && m > 7)
                        calc_flag = 1;
                    else
                        calc_flag = 0;
                    end
                    
                    if coef0(m, n) ~= coef1(m, n) && calc_flag == 1
                        out_mtx(m, n) = abs(coef0(m, n) - coef1(m, n)) / qtable(n, m);
                        fprintf(fid_out_0, "%d\n", coef0(m, n));
                        fprintf(fid_out_1, "%d\n", out_mtx(m, n));
                    end  
                end
            end
        end
    end
end
   

fprintf("finished\n");
fclose('all');
toc