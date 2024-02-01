clear all
clc

width = 1920;
height = 1080;
blk8_num = 80;
min_v = 0;
max_v = 255;
y_mtx = zeros(height, width);
uv_mtx = zeros(height / 2, width);

[fid, msg] = fopen("F:\nfs\part\street_1080p_part.yuv", "r");
if fid == -1
    disp(msg);
    return;
end

[fid_out, msg] = fopen("D:\code\video_learning\matlab\dct_spectrum\output\coef_test_r81.yuv", "w");
if fid_out == -1
    disp(msg);
    return;
end

y_mtx = fread(fid, [width, height]);
uv_mtx = fread(fid, [width, height / 2]);
out_y = y_mtx';
blk16 = [
   212   212   212   212   212   212   212   212   212   212   212   212   212   212   212   212
   212   211   211   211   211   211   211   211   211   211   211   211   211   211   211   212
   212   211   211   211   211   211   211   211   211   211   211   211   211   211   211   210
   212   211   211   211   211   211   211   211   211   211   211   211   211   211   210   212
   211   211   211   211   211   211   211   211   211   211   211   211   210   211   213   201
   211   211   211   211   211   211   211   211   211   211   210   210   213   208   169   114
   211   211   211   211   211   211   211   211   211   210   212   214   189   132    98   130
   211   211   211   211   211   211   211   210   211   215   203   152   104   116   174   212
   211   211   211   211   211   210   209   213   213   176   117    99   148   202   213   209
   211   211   211   210   209   214   216   193   138    97   125   185   216   210   209   211
   211   211   209   210   218   207   159   110   108   164   209   212   209   211   212   211
   211   210   213   213   181   119    97   144   199   215   209   210   212   212   211   211
   212   214   192   136    97   122   179   209   210   208   211   212   211   211   212   212
   203   156   107   109   162   208   213   210   211   212   212   211   211   211   212   212
   120   103   143   197   215   209   209   211   211   211   211   212   212   212   212   212
   120   181   213   211   208   210   211   211   211   211   212   212   212   212   212   212    
];


for k = 1:16:height 
    for j = 1:16:width
        if (j + 15) <= 1920 && (k + 15) <= 1080
            out_y(k:k+15, j:j+15) = blk16;
        end
    end
end


count = fwrite(fid_out, out_y', 'uint8');
count = count + fwrite(fid_out, uv_mtx, 'uint8');

fprintf("fwrite %d bytes\n", count);
fclose(fid);
fclose(fid_out);