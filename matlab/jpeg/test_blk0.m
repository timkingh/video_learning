clear all
clc
format long g

zigzag_order = [
        0,  1,  5,  6, 14, 15, 27, 28;
        2,  4,  7, 13, 16, 26, 29, 42;
        3,  8, 12, 17, 25, 30, 41, 43;
        9, 11, 18, 24, 31, 40, 44, 53;
        10, 19, 23, 32, 39, 45, 52, 54;
        20, 22, 33, 38, 46, 51, 55, 60;
        21, 34, 37, 47, 50, 56, 59, 61;
        35, 36, 48, 49, 57, 58, 62, 63;
 ];

zsantoraster = [
        0,  1,  8, 16,  9,  2,   3, 10;
        17, 24, 32, 25, 18, 11,  4,  5;
        12, 19, 26, 33, 40, 48, 41, 34;
        27, 20, 13,  6,  7, 14, 21, 28;
        35, 42, 49, 56, 57, 50, 43, 36;
        29, 22, 15, 23, 30, 37, 44, 51;
        58, 59, 52, 45, 38, 31, 39, 46;
        53, 60, 61, 54, 47, 55, 62, 63
];

% DCT coef, 3 bits precision
ymtx = [
    5835 -482 -179   73  123   26  -59  -64;
    -444  290  106  -43  -68  -15   33   36;
    -181  122   58  -27  -33   -8   16   12;
    75  -41  -22   13    7    2    0   -1;
    121  -70  -31   11   17    3  -10   -6;
    28  -20   -9    1    6    7   -4   -4;
    -64   35   14   -2   -9   -3    2    1;
    -64   35   10    1   -9   -1    1    4;
];

hi_ret = [
    1753 -60 -22   9  15   3  -7  -8; 
    -56  36  13  -5  -9  -2   4   4; 
    -23  15   7  -3  -4  -1   2   2; 
      9  -5  -3   2   1   0   0   0; 
     15  -9  -4   1   2   0  -2   0; 
      3  -3  -1   0   0   0   0   0; 
     -8   4   2   0  -2   0   0   0; 
     -8   4   2   0  -2   0   0   0 
];

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

diff = (abs(hi_ret) ./ luma_quant_mtx)  - abs(ymtx) / 8;

%cmp_ret = diff > 0

for m = 1:8
    for n = 1:8
        zz = zsantoraster(m, n);
        value = diff(floor(zz / 8) + 1, rem(zz, 8) + 1);
        disp(value);
        %fprintf("zz %d (%d, %d)\n", zz, floor(zz / 8), rem(zz, 8));
        %fprintf("%d ", value);
    end
    %fprintf("\n");
end




