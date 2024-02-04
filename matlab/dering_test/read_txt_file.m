clear all
clc
tic

width = 1920;
height = 1080;
qtable = [
     14  10   9  14  22  36  46  55 
     11  11  13  17  23  52  54  50 
     13  12  14  22  36  51  62  50 
     13  15  20  26  46  78  72  56 
     16  20  33  50  61  98  93  69 
     22  32  50  58  73  94 102  83 
     44  58  70  78  93 109 108  91 
     65  83  86  88 101  90  93  89 
];

max_bias = zeros(8, 8, 20);
coef_cnt = zeros(8, 8);

file_name = ".txt";
path_name = ".\input_file\jpg_dec_street_1080p_part_hisi_r73_pos_384_152";
xy_path = path_name + file_name;
fid_out_xy = fopen(xy_path, 'w');
ret_mtx = zeros(2048, 1);

for quant_value = 1:1:1
    qt_val = quant_value;
    qtable = ones(8, 8) .* qt_val;
    fprintf("qf %d\n", qt_val);

    file_path = ".\dr0\madi.txt";
    [fid_dering0, msg] = fopen(file_path, 'r');
    if fid_dering0 == -1
        disp(msg);
        return;
    end

    file_path = ".\dr1\madi.txt";
    [fid_dering1, msg] = fopen(file_path, 'r');
    if fid_dering1 == -1
        disp(msg);
        return;
    end

    for row = 1:8:height
        for col = 1:8:width
            for idx = 0:0
                pos_x = col - 1 + rem(idx, 2) * 8;
                pos_y = row - 1 + floor(idx / 2) * 8;
                ret0 = fscanf(fid_dering0, 'pos(%d, %d) %d\n', 3);
                ret1 = fscanf(fid_dering1, 'pos(%d, %d) %d\n', 3);         
                ret_diff = (ret0(3, 1) - ret1(3, 1)) + 1024;
                ret_mtx(ret_diff + 1, 1) = ret_mtx(ret_diff + 1, 1) + 1;
                if (ret0(3, 1)) < ret1(3, 1)
                    fprintf("pos(%d, %d)\n", ret0(1, 1), ret0(2, 1));
                end
            end
        end
    end

    fclose(fid_dering0);
    fclose(fid_dering1);
end

ret_mtx(1025, 1) = 0;

% stem(1:2048, ret_mtx');

fprintf("finished\n");
fclose(fid_out_xy);
toc