clear all 
clc

coef = [
    "62.txt";
    "63.txt";
    "64.txt";
    "72.txt";
    "73.txt";
    "74.txt";
];

x1_str = ".\log_2\x1_r44_A";
y1_str = ".\log_2\y1_r44_A";

file_list = [
    x1_str + coef(1, 1), y1_str + coef(1, 1), "sk";
    x1_str + coef(2, 1), y1_str + coef(2, 1), "*b"; 
    x1_str + coef(3, 1), y1_str + coef(3, 1), "or"; 
    x1_str + coef(4, 1), y1_str + coef(4, 1), "+c"; 
    x1_str + coef(5, 1), y1_str + coef(5, 1), "dm"; 
    x1_str + coef(6, 1), y1_str + coef(6, 1), "xg"; 
];

[rows, cols] = size(file_list);

for m = 1:rows
    x1_path = file_list(m, 1);
    y1_path = file_list(m, 2);
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

    stem(x1, y1, file_list(m, 3));

    fclose('all');

    hold on
end
