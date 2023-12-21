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

x1_path = ".\log\x1_r21.txt";
y1_path = ".\log\y1_r21.txt";
fid_var = fopen('.\log\out_var_street_720p.txt', 'r');
fid_out_0 = fopen(x1_path, 'w');
fid_out_1 = fopen(y1_path, 'w');

qtable = ones(8, 8) * 10;

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
                    if (n == 8 && m == 2) % A73
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
   
fclose('all');
[fid_x1, msg] = fopen(x1_path, "r");
if fid_x1 == -1
    disp(msg);
    return;
end

[fid_y1, msg] = fopen(y1_path, "r");
if fid_y1 == -1
    disp(msg);
    return;
end

x1 = fscanf(fid_x1, '%d');
y1 = fscanf(fid_y1, '%d');

stem(x1, y1, 'filled');

fprintf("finished\n");
fclose('all');
toc