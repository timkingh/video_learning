function blk_sse = calc_sse(mtx1, mtx2, m, n);
% mtx1, mtx2: m*n

sum_sse = 0;

for row = 1:m
    for col = 1:n
        pix_diff = mtx1(row, col) - mtx2(row, col);
        sum_sse = sum_sse + pix_diff * pix_diff;
    end
end

blk_sse = sum_sse;