clear all
clc

T = dctmtx(8);

width = 1280;
height = 720;

[fid, msg] = fopen("F:\nfs\street_720p_out_42.yuv", "r");
if fid == -1
    disp(msg);
    return;
end

zscan2raster = [
        0,  1,  8, 16,  9,  2,   3, 10, ...
        17, 24, 32, 25, 18, 11,  4,  5, ...
        12, 19, 26, 33, 40, 48, 41, 34, ...
        27, 20, 13,  6,  7, 14, 21, 28, ...
        35, 42, 49, 56, 57, 50, 43, 36, ...
        29, 22, 15, 23, 30, 37, 44, 51, ...
        58, 59, 52, 45, 38, 31, 39, 46, ...
        53, 60, 61, 54, 47, 55, 62, 63
];

y_mtx = fread(fid, [width, height]);

out_mtx = y_mtx';

fclose(fid);

out_blk = ones(8, 8, 2);

for index = 1:2
    blk = out_mtx(1:8, 465:472);
    if index == 1
        blk(1:2:7, :) = 0;
        blk(2:2:8, :) = 120;
    else
        blk(:, 1:2:7) = 120;
        blk(:, 2:2:8) = 0;
    end
    out_blk(:, :, index) = blk

    coef = T * (blk - 128) * T';
    coef_r = round(coef)

    coef_reshape = reshape(coef_r', 1, 64);
    coef_zz = zeros(1, 64);

    for m = 1:64
        coef_zz(1, m) = (coef_reshape(1, zscan2raster(1, m) + 1));
    end
    
    grid on
    if index == 1
        plot(0:1:63, coef_zz, '-ro');
    else 
        plot(0:1:63, coef_zz, '--k*');
    end
    hold on
end






 
 