% create YUV files
clc
clear all

T = dctmtx(8);

luma_quant_mtx = [
    1, 1, 1, 1, 1, 1, 1, 1;
    1, 1, 1, 1, 1, 1, 1, 1;
    1, 1, 1, 1, 1, 1, 1, 1;
    1, 1, 1, 1, 1, 2, 2, 1;
    1, 1, 1, 1, 1, 2, 2, 2;
    1, 1, 1, 1, 2, 2, 2, 2;
    1, 1, 2, 2, 2, 2, 2, 2;
    1, 2, 2, 2, 2, 2, 2, 2;
];

blk8_num = 5;
dct_coef = zeros(8, 8, blk8_num);
org_pic = zeros(8, 8, blk8_num);

cnt_a = 0;
cnt_b = 0;

% calc YUV
for k = 1:5
    for m = 1:8
        for n = 1:8
            if  luma_quant_mtx(m, n) == 1
                dct_coef(m, n, k) = cnt_a;
                cnt_a = cnt_a + 1;
            else
                dct_coef(m, n, k) = cnt_b;
                cnt_b = cnt_b + 1;
            end
        end
    end
    
    org_pic(:,:,k) = round(T' * dct_coef(:,:,k) * T) + 128;
end


% output YUV
fid = fopen("jpeg_test_720p_21.yuv", "w");

width = 1280;
height = 720;
start_value = 0;

y_mtx = ones([width, height]);
uv_mtx = ones(width, height / 2) .* 100;

pic_blk8x8 = [
     208 208 208 209 209 209 209 209;
     208 208 208 209 209 209 209 210;
     207 207 206 211 211 211 211 211;
     208 208 210 232 235 234 234 234;
     208 208 210 234 239 238 237 236;
     208 208 210 233 237 237 236,236;
     208 208 210 233 236 236 236,236;
     208 209 210 232 235 235 235,236
];


y_mtx = gen_yuv_for_dc_coef_test_blk8(width, height, -1024);
%y_mtx = gen_yuv_from_blk8(width, height);
%y_mtx = randi([0, 255], height, width);
%y_mtx = gen_yuv_for_coef_test(width, height, -23);
uv_mtx = randi([0, 255], height / 2, width);

count = fwrite(fid, y_mtx', 'uint8');
%count = count + fwrite(fid, uv_mtx', 'uint8');

fprintf("fwrite %d bytes\n", count);

fclose(fid);