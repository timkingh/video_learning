
file = 'D:\code\video_learning\matlab\filter_md\test_854x480_yuv400.yuv';
fid = fopen(file, 'rb');
width = 854;
height = 480;
frame_num = 1;

filter_radius = 25;
filter_sigma = filter_radius;
filter_kernel = fspecial('gaussian', filter_radius, filter_sigma);

for i = 1:frame_num
    y = fread(fid, [width, height], 'ubit8')';
    
    y_filter = imfilter(y, filter_kernel, 'replicate');
    
    y_detail = abs(y - y_filter);
    
    figure, imshow(y/255)
    figure, imshow(y_filter/255)
    figure, imshow(y_detail/255)
end

fclose(fid);