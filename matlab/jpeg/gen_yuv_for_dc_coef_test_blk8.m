function pic = gen_yuv_for_dc_coef_test_blk8(width, height, start)
% width : pic width
% height: pic height
% start : first dc coef
% end   : last dc coef

pic = zeros(height, width);
dc_coef = start - 1;
coef_range(1, 2) = 1016;

for m = 1:8:height
    for n = 1:8:width
        coef_range(1, 1) = dc_coef + 1;
        dc_coef = min(coef_range);
        pic((m+0):(m+7), (n+0):(n+7)) = get_yuv_blk8_from_dc_coef(dc_coef);
    end
end
