clear all
clc

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

dct_coef = [
     0   -60   -22     9    15     3    -7    -8;
   -56    36    13    -5    -9    -2     4     4;
   -23    15     7    -3    -4    -1     2     2;
     9    -5    -3     2     1     0     0     0;
    15    -9    -4     1     2     0    -1    -1;
     3    -3    -1     0     1     1     0     0;
    -8     4     2     0    -1     0     0     0;
    -8     4     1     0    -1     0     0     0;
];

cnt_a = 0;
cnt_b = 0;

for m = 1:8
    for n = 1:8
        if  luma_quant_mtx(m, n) == 1
            dct_coef(m, n) = cnt_a;
            cnt_a = cnt_a + 1;
        else
            dct_coef(m, n) = cnt_b;
            cnt_b = cnt_b + 1;
        end
    end
end


dct_coef

org_pic = round(T' * dct_coef * T) + 128

dct_coef_second_float = T * (org_pic - 128) * T'

dct_coef_second_int = round(dct_coef_second_float)

diff_coef = dct_coef - dct_coef_second_int

