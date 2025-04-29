clear all
clc

% 参数配置
yuv_file = 'D:\code\video_learning\matlab\filter_md\yuv400\31705_854x480_yuv400.yuv';    % YUV400文件路径
width = 854;               % 图像宽度（需根据实际情况修改）
height = 480;              % 图像高度（需根据实际情况修改）
block_size = 16;           % DCT块尺寸
cutoff_ratio = 0.125 / 2;       % 低频截止比例

% 读取YUV400文件（仅读取Y分量）
fid = fopen(yuv_file, 'r');
yuv_data = fread(fid, [width, height], 'uint8')'; % 读取Y分量并转置为HxW
fclose(fid);

% 预处理
img = im2double(yuv_data); % 转换为[0,1]范围

% 自动计算截止尺寸
cutoff_size = floor(block_size * cutoff_ratio);
mask = triu(ones(block_size), cutoff_size);  % 创建三角形掩膜

% 分块DCT处理
img_filtered = blockproc(img, [block_size block_size], ...
    @(b) blockProcessing(b.data, mask), ...
    'PadPartialBlocks', true, ...
    'PadMethod', 'symmetric');

% 后处理
y_filtered = im2uint8(mat2gray(img_filtered)); % 转换为uint8的Y分量

% 显示结果
figure;
subplot(1,2,1), imshow(yuv_data,[]), title('原始Y分量');
subplot(1,2,2), imshow(y_filtered,[]), title(sprintf('块尺寸%d 截止%d',block_size,cutoff_size));

% 可选：保存处理后的Y分量为新YUV文件
fid = fopen('output.yuv','w');
fwrite(fid, y_filtered', 'uint8'); % 注意需要转置恢复原始存储顺序
fclose(fid);

% 分块处理函数
function out = blockProcessing(block, mask)
    dct_block = dct2(block);          % 块DCT变换
    dct_filtered = dct_block .* mask; % 高频分量保留
    out = idct2(dct_filtered);        % 块逆DCT
end