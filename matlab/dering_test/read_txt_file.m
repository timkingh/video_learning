clear all
clc
tic

width = 1280;
height = 720;
qtable = ones(8, 8) * 10;

for jj = 0:7
    for kk = 0:7
        file_name = num2str(kk) + ".txt";
        x1_path = ".\log_7\x1_r49_A" + num2str(jj) + file_name;
        y1_path = ".\log_7\y1_r49_A" + num2str(jj) + file_name;
        fid_out_0 = fopen(x1_path, 'w');
        fid_out_1 = fopen(y1_path, 'w');
        fprintf("jj %d kk %d\n", jj, kk);
        [fid_dering0, msg] = fopen('.\log_7\jpg_dec_street_720p_out_40_hisi_r40_qt10_dering0.txt', 'r');
        if fid_dering0 == -1
            disp(msg);
            return;
        end

        [fid_dering1, msg] = fopen('.\log_7\jpg_dec_street_720p_out_40_hisi_r40_qt10_dering1.txt', 'r');
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
                                fprintf(fid_out_0, "%d\n", coef0(m, n));
                                fprintf(fid_out_1, "%d\n", out_mtx(m, n));
                            end  
                        end
                    end
                end
            end
        end
        
        fclose('all');
    end
end

fprintf("finished\n");
toc