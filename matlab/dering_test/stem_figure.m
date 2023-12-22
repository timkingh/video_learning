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


file_list = [
    ".\log_1\x1_r39_A" + coef(1, 1), ".\log_1\y1_r39_A" + coef(1, 1), "sk";
    ".\log_1\x1_r39_A" + coef(2, 1), ".\log_1\y1_r39_A" + coef(2, 1), "*b"; 
    ".\log_1\x1_r39_A" + coef(3, 1), ".\log_1\y1_r39_A" + coef(3, 1), "or"; 
    ".\log_1\x1_r39_A" + coef(4, 1), ".\log_1\y1_r39_A" + coef(4, 1), "+c"; 
    ".\log_1\x1_r39_A" + coef(5, 1), ".\log_1\y1_r39_A" + coef(5, 1), "dm"; 
    ".\log_1\x1_r39_A" + coef(6, 1), ".\log_1\y1_r39_A" + coef(6, 1), "xg"; 
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
