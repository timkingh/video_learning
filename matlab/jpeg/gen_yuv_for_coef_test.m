function pic = gen_yuv_for_coef_test(width, height, coef)
% width : pic width
% height: pic height
% start : first dc coef
% end   : last dc coef

pic = zeros(height, width);
y_mtx = zeros(8, 8);
ret = 0;


for m = 1:16:height
    for n = 1:16:width
        fprintf("(%d, %d) start:\n", n, m);
        while ret == 0
            [y_mtx, ret] = gen_blk8(coef);
        end
        pic((m+0):(m+7), (n+0):(n+7)) = y_mtx;
        ret = 0;
        
        while ret == 0
            [y_mtx, ret] = gen_blk8(coef);
        end
        pic((m+0):(m+7), (n+8):(n+15)) = y_mtx;
        ret = 0;
        
        while ret == 0
            [y_mtx, ret] = gen_blk8(coef);
        end
        pic((m+8):(m+15), (n+0):(n+7)) = y_mtx;
        ret = 0;
        
        while ret == 0
            [y_mtx, ret] = gen_blk8(coef);
        end
        pic((m+8):(m+15), (n+8):(n+15)) = y_mtx;
    end
end
