clear all
clc
tic

width = 1920;
height = 1080;
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

max_bias = zeros(8, 8, 20);
coef_cnt = zeros(8, 8);

for quant_value = 10:1:10
    qt_val = quant_value;
    qtable = ones(8, 8) .* qt_val;

    file_name = ".txt";
    path_name = ".\input_file\jpg_dec_street_1080p_part_hisi_r73_qf" + num2str(qt_val) + "_A";
    xy_path = path_name + file_name;
%     fid_out_xy = fopen(xy_path, 'w');

    file_path = ".\input_file\jpg_dec_street_1080p_part_hisi_r73_qf" + num2str(qt_val) + "_dering0.txt";
    [fid_dering0, msg] = fopen(file_path, 'r');
    if fid_dering0 == -1
        disp(msg);
        return;
    end

    file_path = ".\input_file\jpg_dec_street_1080p_part_hisi_r73_qf" + num2str(qt_val) + "_dering1.txt";
    [fid_dering1, msg] = fopen(file_path, 'r');
    if fid_dering1 == -1
        disp(msg);
        return;
    end

    for row = 1:16:height
        for col = 1:16:width
            for idx = 0:3
                pos_x = col - 1 + rem(idx, 2) * 8;
                pos_y = row - 1 + floor(idx / 2) * 8;
                fscanf(fid_dering0, 'pos(%d, %d)\nplane 0 dct coef:\n');
                fscanf(fid_dering1, 'pos(%d, %d)\nplane 0 dct coef:\n');
                coef0 = fscanf(fid_dering0, '%d', [8, 8]);
                coef1 = fscanf(fid_dering1, '%d', [8, 8]);
                fscanf(fid_dering0, '\n');
                fscanf(fid_dering1, '\n');                 

                if pos_x == 392 && pos_y == 144
                   for n = 1:8 % row
                        for m = 1:8 % col
                            if coef0(m, n) ~= coef1(m, n)
                                coef_cnt(m, n) = coef_cnt(m, n) + 1;
                            end  
                        end
                    end
                end
                

            end
        end
    end

    fclose('all');
end

coef_cnt'

fprintf("finished\n");
toc