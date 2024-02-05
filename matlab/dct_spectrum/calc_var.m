function [blk_ave, blk_var, blk_madi] = calc_var(mtx, m, n);

% mtx: [m, n]
sum = 0;

for r = 1:m
    for c = 1:n
        sum = sum + mtx(r, c);
    end
end

ave = round(sum / (m * n));
blk_ave = ave;


sum_var = 0;
sum_madi = 0;
for r = 1:m
    for c = 1:n
        sum_var = sum_var + (mtx(r, c) - ave) * (mtx(r, c) - ave);
        sum_madi = sum_madi + abs(mtx(r, c) - ave);
    end
end

blk_var = round(sum_var / (m * n));
blk_madi = sum_madi;
