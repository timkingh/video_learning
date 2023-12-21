clear all 
clc


[fid_x1, msg] = fopen("D:\code\video_learning\matlab\dering_test\log\x1_r6.txt", "r");
if fid_x1 == -1
    disp(msg);
    return;
end

[fid_y1, msg] = fopen("D:\code\video_learning\matlab\dering_test\log\y1_r6.txt", "r");
if fid_y1 == -1
    disp(msg);
    return;
end

x1 = fscanf(fid_x1, '%d');
y1 = fscanf(fid_y1, '%d');

stem(x1, y1, 'filled');

fclose('all');