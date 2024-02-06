function out_mtx = open_yuv_file(file_path, width, height, fmt, seek);

% file_path: height * width
% width:
% height:
% fmt: 'yuv420p', 'yuv422p'
% seek: start from zero
% out_mtx: height * width

out_mtx = zeros(height, width);
tmp_mtx = zeros(width, height);

[fid, msg] = fopen(file_path, 'r');
if fid == -1
    disp(msg);
    return;
end

% y_size = width * height;
uv_size = width * height / 2;

if strcmp(fmt, 'yuv422p')
    uv_size = width * height;
end

for iter = 0:seek
    tmp_mtx = fread(fid, [width, height]);
    fread(fid, uv_size);
end

out_mtx = tmp_mtx';

fclose(fid);