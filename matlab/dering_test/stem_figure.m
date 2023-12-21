clear all 
clc

x1_path = ".\log\x1_r9.txt";
y1_path = ".\log\y1_r9.txt";
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

stem(x1, y1, 'sk');

fclose('all');

hold on

x1_path = ".\log\x1_r10.txt";
y1_path = ".\log\y1_r10.txt";
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

stem(x1, y1, 'or');

fclose('all');


hold on

x1_path = ".\log\x1_r7.txt";
y1_path = ".\log\y1_r7.txt";
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

stem(x1, y1, '*b');

fclose('all');
