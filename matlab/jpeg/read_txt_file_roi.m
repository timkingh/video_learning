clear all
clc

width = 1280;
height = 720;

zsantoraster = [
        0,  1,  8, 16,  9,  2,   3, 10, ...
        17, 24, 32, 25, 18, 11,  4,  5, ...
        12, 19, 26, 33, 40, 48, 41, 34, ...
        27, 20, 13,  6,  7, 14, 21, 28, ...
        35, 42, 49, 56, 57, 50, 43, 36, ...
        29, 22, 15, 23, 30, 37, 44, 51, ...
        58, 59, 52, 45, 38, 31, 39, 46, ...
        53, 60, 61, 54, 47, 55, 62, 63
];

fid_roi0 = fopen('jpg_dec_street_hisi_r4_m3_roi0_raster.txt', 'r');
fid_roi1 = fopen('jpg_dec_street_hisi_r4_m3_roi1_raster.txt', 'r');
fid_out = fopen('dct_cmp_out.txt', 'w');

blk_num = 0;
is_luma = 1;

for row = 1:16:height
    for col = 1:16:width
        for idx = 0:5 
            if idx < 4  % Y
                pos_x = col - 1 + rem(idx, 2) * 8;
                pos_y = row - 1 + floor(idx / 2) * 8;
                is_luma = 1;
            else        % UV
                pos_x = (col - 1) / 2;
                pos_y = (row - 1) / 2;
                is_luma = 0;
            end
            
            coef0 = fscanf(fid_roi0, '%d', [8, 8]);
            coef1 = fscanf(fid_roi1, '%d', [8, 8]);
            coef0_r = reshape(coef0, 64, 1);
            coef1_r = reshape(coef1, 64, 1);
            
            if (pos_y < 704 && is_luma) || (pos_y < 352 && (is_luma == 0))
                for m = 1:64
                    r_idx = zsantoraster(1, m) + 1;
                    if m <= 17 && (coef0_r(r_idx, 1) ~= coef1_r(r_idx, 1))
                        fprintf("non-roi luma error-1\n");
                    end

                    if m > 17 && (coef1_r(r_idx, 1) ~= 0)
                        fprintf("non-roi luma error-2\n");
                    end
                end
            else
                if isequal(coef0, coef1) ~= 1
                    fprintf("roi luma error");
                end
            end
        end
        
        blk_num = blk_num + 6;
    end
end


fclose(fid_roi0);
fclose(fid_roi1);
fclose(fid_out);

fprintf("blk_num %d\n", blk_num);