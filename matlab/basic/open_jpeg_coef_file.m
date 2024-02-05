function [coef_mtx, pos_mtx] = open_jpeg_coef_file(file_path, width, height);

% file_path:
% width:
% height:
% coef_mtx: 8 * 8 * blk8_num
% pos_mtx : blk8_num * 2

[fid, msg] = fopen(file_path, 'r');
if fid == -1
    disp(msg);
    return;
end

blk8_num = width / 8 * height / 8;
pos_mtx = zeros(blk8_num, 2);
coef_mtx = zeros(8, 8, blk8_num);

for iter = 1:blk8_num
    blk_pos = fscanf(fid, 'pos(%d, %d)\n', 2);
    pos_mtx(iter, :) = blk_pos;
    
    fscanf(fid, 'plane %d dct coef:\n', 1);
    blk8_coef = fscanf(fid, '%d', [8 8]);
    coef_mtx(:, :, iter) = blk8_coef';
    
    fscanf(fid, '\n\n');
end    
    
    
fclose(fid);