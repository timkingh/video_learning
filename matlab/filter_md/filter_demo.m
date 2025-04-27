clc
clear


file = 'D:\code\video_learning\matlab\filter_md\test_854x480_yuv400.yuv';
file_filt = 'D:\code\video_learning\matlab\filter_md\test_854x480_yuv400_filt.yuv';
file_md = 'D:\code\video_learning\matlab\filter_md\test_854x480_yuv400_md.yuv';

fid = fopen(file, 'rb');
if fid == -1
    error('fopen %s file', file);
end

fid_filt = fopen(file_filt, 'w+');
if fid_filt == -1
    error('fopen %s file', file_filt);
end

fid_md = fopen(file_md, 'w+');
if fid_md == -1
    error('fopen %s file', file_md);
end


width = 854;
height = 480;
frame_num = 495;

filter_radius = 25;
filter_sigma = filter_radius;
filter_kernel = fspecial('gaussian', filter_radius, filter_sigma);

for i = 1:frame_num
    y = fread(fid, [width, height], 'ubit8')';
    
    y_filter = imfilter(y, filter_kernel, 'replicate');
    
    y_detail = abs(y - y_filter);
    y_detail_int = uint8(y_detail);
    
    %figure, imshow(y/255)
    %figure, imshow(y_filter/255)
    %figure, imshow(y_detail/255)
    
    fwrite(fid_filt, uint8(y_filter'), 'uint8');
    fwrite(fid_md, uint8(y_detail_int'), 'uint8');
end

fclose(fid);
fclose(fid_filt);
fclose(fid_md);