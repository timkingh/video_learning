clear all
clc

coef = [
    ".\log\log_12\xy_r57_qt10_A31.txt";
    ".\log\log_12\xy_r57_qt10_A32.txt";
    ".\log\log_12\xy_r57_qt10_A33.txt";
    ".\log\log_12\xy_r57_qt10_A34.txt";
    ".\log\log_12\xy_r57_qt10_A35.txt";
    ".\log\log_12\xy_r57_qt10_A36.txt";
    ".\log\log_12\xy_r57_qt10_A37.txt";
    ".\log\log_12\xy_r57_qt10_A45.txt";
    ".\log\log_12\xy_r57_qt10_A46.txt";
];

file_list = [
    coef(1, 1), "sk";
    coef(2, 1), "*b";
    coef(3, 1), "or";
    coef(4, 1), "+c";
    coef(5, 1), "dm";
    coef(6, 1), "xg";
    coef(7, 1), "py";
    coef(8, 1), "vw";
    coef(9, 1), "<r";
];

[rows, cols] = size(file_list);

for m = 1:rows
    xy_path = file_list(m, 1);
    [fid_xy, msg] = fopen(xy_path, "r");
    if fid_xy == -1
        disp(msg);
        return;
    end

    xy = fscanf(fid_xy, '%d');
    [xy_r, xy_c] = size(xy);
    xy_reshape = reshape(xy, 4, xy_r / 4);

    stem(xy_reshape(1, :), xy_reshape(3, :), file_list(m, 2));

    fclose('all');

    hold on
end

grid on
