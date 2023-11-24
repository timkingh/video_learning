function [out_y, ret] = gen_blk8(dct_coef_value)

T = dctmtx(8);
test_num = 500;
ret = 0;
out_y = zeros(8, 8);

% for m = 0:test_num
%     y_mtx = randi([0, 255], 8, 8);
%     dct_coef = T * (y_mtx - 128) * T';
%     dct_coef_int = round(dct_coef);
%     if dct_coef_int(0, 0) == dct_coef_value
%         out_y(1:8, 1:8) = y_mtx(1:8, 1:8);
%         ret = 1;
%         break;
%     end
% end

for m = 0:test_num
    y_mtx = randi([0, 255], 8, 8);
    sum = 0;
    for j = 1:8
        for k = 1:8
            sum = sum + y_mtx(j, k) - 128;
        end
    end
    
    if sum >= 6205 && sum <= 6212
        out_y(1:8, 1:8) = y_mtx(1:8, 1:8);
        ret = 1;
        break;
    end
end