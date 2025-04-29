clear all
clc

% ��������
yuv_file = 'D:\code\video_learning\matlab\filter_md\yuv400\31705_854x480_yuv400.yuv';    % YUV400�ļ�·��
width = 854;               % ͼ���ȣ������ʵ������޸ģ�
height = 480;              % ͼ��߶ȣ������ʵ������޸ģ�
block_size = 16;           % DCT��ߴ�
cutoff_ratio = 0.125 / 2;       % ��Ƶ��ֹ����

% ��ȡYUV400�ļ�������ȡY������
fid = fopen(yuv_file, 'r');
yuv_data = fread(fid, [width, height], 'uint8')'; % ��ȡY������ת��ΪHxW
fclose(fid);

% Ԥ����
img = im2double(yuv_data); % ת��Ϊ[0,1]��Χ

% �Զ������ֹ�ߴ�
cutoff_size = floor(block_size * cutoff_ratio);
mask = triu(ones(block_size), cutoff_size);  % ������������Ĥ

% �ֿ�DCT����
img_filtered = blockproc(img, [block_size block_size], ...
    @(b) blockProcessing(b.data, mask), ...
    'PadPartialBlocks', true, ...
    'PadMethod', 'symmetric');

% ����
y_filtered = im2uint8(mat2gray(img_filtered)); % ת��Ϊuint8��Y����

% ��ʾ���
figure;
subplot(1,2,1), imshow(yuv_data,[]), title('ԭʼY����');
subplot(1,2,2), imshow(y_filtered,[]), title(sprintf('��ߴ�%d ��ֹ%d',block_size,cutoff_size));

% ��ѡ�����洦����Y����Ϊ��YUV�ļ�
fid = fopen('output.yuv','w');
fwrite(fid, y_filtered', 'uint8'); % ע����Ҫת�ûָ�ԭʼ�洢˳��
fclose(fid);

% �ֿ鴦����
function out = blockProcessing(block, mask)
    dct_block = dct2(block);          % ��DCT�任
    dct_filtered = dct_block .* mask; % ��Ƶ��������
    out = idct2(dct_filtered);        % ����DCT
end