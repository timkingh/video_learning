clear all
close all
% clc
tic


blk8_num = 3;
min_var = 10000000000;
T = dctmtx(8);
min_blk8 = zeros(8, 8, blk8_num);
var_sets = ones(1, blk8_num) .* 10000000;
min_coef_r = zeros(8, 8);
blk8 = zeros(8, 8);

for idx = 1:1
    pix_val = randi([0, 255], 1, 16);
    sort_val = sort(pix_val);
    
    for row = 0:7
        for col = 0:7
            blk8(row + 1, col + 1) = sort_val(1, row + floor(col / 2) + 1);
        end
    end    
    
    blk8
    coef = T * (blk8 - 128) * T';
    coef_r = round(coef);
    coef_r(1, 1) = 0;
    figure;
    imagesc(abs(coef_r));
    colorbar;
%     fprintf("figure %d\n", idx);
%     fprintf("\n");
    
%     sum_coef = 0;
%     for r = 7:8
%         for c = 6:8
%             sum_coef = sum_coef + abs(coef_r(r, c));
%         end
%     end
%     
%     ave_coef = round(sum_coef / 6);
%     var_coef = 0;
%     for r = 7:8
%         for c = 1:8
%             var_coef = var_coef + (abs(coef_r(r, c)) - ave_coef) .* (abs(coef_r(r, c)) - ave_coef);
%         end
%     end
%     
%     for kk = 1:blk8_num
%         if var_coef < var_sets(1, kk)
%             var_sets(1, kk) = var_coef;
%             min_blk8(:, :, kk) = blk8;
%             var_coef
%             coef_r
%             break;
%         end
%     end
end

% save("blk8_1.mat", "min_blk8");
toc