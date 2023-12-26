clear all 
clc

coef = [
    "31.txt";
    "32.txt";
    "33.txt";
    "07.txt";
    "47.txt";
    "50.txt";
    "07.txt";
    "47.txt";
    "50.txt";
];

xy_str = ".\log\log_12\xy_r56_A";

file_list = [
    xy_str + coef(1, 1), "sk";
    xy_str + coef(2, 1), "*b"; 
    xy_str + coef(3, 1), "or"; 
    xy_str + coef(4, 1), "+c"; 
    xy_str + coef(5, 1), "dm"; 
    xy_str + coef(6, 1), "xg"; 
    xy_str + coef(7, 1), "py"; 
    xy_str + coef(8, 1), "vw"; 
    xy_str + coef(9, 1), "<r"; 
];

[rows, cols] = size(file_list);

for m = 1:1
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
