clear all
% clc
tic


blk8_num = 1000;
min_var = 10000000000;
T = dctmtx(8);
min_blk8 = zeros(8, 8, blk8_num);
var_sets = ones(1, blk8_num) .* 10000000;
min_coef_r = zeros(8, 8);

for idx = 1:100000000
    blk8 = randi([0, 255], 8, 8);
    coef = T * (blk8 - 128) * T';
    coef_r = round(coef);
    
    sum_coef = 0;
    for r = 7:8
        for c = 1:8
            sum_coef = sum_coef + abs(coef_r(r, c));
        end
    end
    
    ave_coef = round(sum_coef / 16);
    var_coef = 0;
    for r = 7:8
        for c = 1:8
            var_coef = var_coef + (abs(coef_r(r, c)) - ave_coef) .* (abs(coef_r(r, c)) - ave_coef);
        end
    end
    
    for kk = 1:blk8_num
        if var_coef < var_sets(1, kk)
            var_sets(1, kk) = var_coef;
            min_blk8(:, :, kk) = blk8;
            break;
        end
    end
end

save("blk8.mat", "min_blk8");
toc