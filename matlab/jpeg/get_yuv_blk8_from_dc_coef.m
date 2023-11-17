function blk8 = get_yuv_blk8_from_dc_coef(dc_coef)
% dc_coef: [-1024, 1016]

blk8 = zeros(8, 8);

abs_dc_coef = abs(dc_coef);

if dc_coef >= 0
    lines_floor = floor(abs_dc_coef / 127);
    lines = ceil(abs_dc_coef / 127);
else 
    lines_floor = floor(abs_dc_coef / 128);
    lines = ceil(abs_dc_coef / 128);
end

if dc_coef >= 0
    for row = 1:lines
        if row <= lines_floor
            blk8(row, :) = 127;
        else        
            blk8(row, :) = ones(1, 8) .* (dc_coef - 127 * (row - 1));
        end
    end
else
    for row = 1:lines
        if row <= lines_floor
            blk8(row, :) = -128;
        else        
            blk8(row, :) = ones(1, 8) .* (dc_coef + 128 * (row - 1));
        end
    end    
end

blk8 = blk8 + 128;