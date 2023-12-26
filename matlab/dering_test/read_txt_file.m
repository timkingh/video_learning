clear all
clc
tic

width = 1280;
height = 720;
% qtable = [
%   3   2   2   3   5   8  10  12
%   2   2   3   4   5  12  12  11
%   3   3   3   5   8  11  14  11
%   3   3   4   6  10  17  16  12
%   4   4   7  11  14  22  21  15
%   5   7  11  13  16  21  23  18
%  10  13  16  17  21  24  24  20
%  14  18  19  20  22  20  21  20
% ];

qtable = ones(8, 8) .* 10;
max_bias = zeros(8, 8);

for jj = 0:7
    for kk = 0:7
        file_name = num2str(kk) + ".txt";
        xy_path = ".\log\log_12\xy_r57_qt10_A" + num2str(jj) + file_name;
        fid_out_xy = fopen(xy_path, 'w');

        [fid_dering0, msg] = fopen('.\log\log_12\jpg_dec_coef_test_r43_v2_hisi_r39_qt10_dering0.txt', 'r');
        if fid_dering0 == -1
            disp(msg);
            return;
        end

        [fid_dering1, msg] = fopen('.\log\log_12\jpg_dec_coef_test_r43_v2_hisi_r39_qt10_dering1.txt', 'r');
        if fid_dering1 == -1
            disp(msg);
            return;
        end
        
        for row = 1:16:height
            for col = 1:16:width
                for idx = 0:3
%                     pos_x = col - 1 + rem(idx, 2) * 8;
%                     pos_y = row - 1 + floor(idx / 2) * 8;
                    coef0 = fscanf(fid_dering0, '%d', [8, 8]);
                    coef1 = fscanf(fid_dering1, '%d', [8, 8]);
                    out_mtx = zeros(8, 8);                     

                    for n = 1:8 % row
                        for m = 1:8 % col
                            if (n == jj + 1 && m == kk + 1)
                                calc_flag = 1;
                            else
                                calc_flag = 0;
                            end

                            if coef0(m, n) ~= coef1(m, n) && calc_flag == 1
                                out_mtx(m, n) = abs(coef0(m, n) - coef1(m, n)) / qtable(n, m);
                                fprintf(fid_out_xy, "%5d %5d %5d %5d\n", coef0(m, n), ...
                                        coef0(m, n) / qtable(n, m), out_mtx(m, n), qtable(n, m));
                                
                                if out_mtx(m, n) > max_bias(jj + 1, kk + 1) 
                                    max_bias(jj + 1, kk + 1) = out_mtx(m, n);
                                end
                            end  
                        end
                    end
                end
            end
        end
        
        fprintf("jj %d kk %d max_bias %d\n", jj, kk, max_bias(jj + 1, kk + 1));
        
        fclose('all');
    end
end

for jj = 1:8
    for kk = 1:8
        fprintf("%4d ", max_bias(jj, kk));
    end
    fprintf("\n");
end
fprintf("\n");

fprintf("finished\n");
toc