clear all
close all
clc
tic

T = dctmtx(8);
qtable = [
     14  10   9  14  22  36  46  55 
     11  11  13  17  23  52  54  50 
     13  12  14  22  36  51  62  50 
     13  15  20  26  46  78  72  56 
     16  20  33  50  61  98  93  69 
     22  32  50  58  73  94 102  83 
     44  58  70  78  93 109 108  91 
     65  83  86  88 101  90  93  89 
];

qtable_delta = [
     0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0
     0 1 0 1 0 0 0 0
     0 0 1 0 0 0 0 0
     1 0 1 0 0 0 0 0
     0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0
];

blk16 = [
   212   212   212   212   212   212   212   212   212   212   212   212   212   212   212   212
   212   211   211   211   211   211   211   211   211   211   211   211   211   211   211   212
   212   211   211   211   211   211   211   211   211   211   211   211   211   211   211   210
   212   211   211   211   211   211   211   211   211   211   211   211   211   211   210   212
   211   211   211   211   211   211   211   211   211   211   211   211   210   211   213   201
   211   211   211   211   211   211   211   211   211   211   210   210   213   208   169   114
   211   211   211   211   211   211   211   211   211   210   212   214   189   132    98   130
   211   211   211   211   211   211   211   210   211   215   203   152   104   116   174   212
   211   211   211   211   211   210   209   213   213   176   117    99   148   202   213   209
   211   211   211   210   209   214   216   193   138    97   125   185   216   210   209   211
   211   211   209   210   218   207   159   110   108   164   209   212   209   211   212   211
   211   210   213   213   181   119    97   144   199   215   209   210   212   212   211   211
   212   214   192   136    97   122   179   209   210   208   211   212   211   211   212   212
   203   156   107   109   162   208   213   210   211   212   212   211   211   211   212   212
   120   103   143   197   215   209   209   211   211   211   211   212   212   212   212   212
   120   181   213   211   208   210   211   211   211   211   212   212   212   212   212   212    
];

blk8 = blk16(1:8, 9:16);
coef_r = zeros(8, 8);
coef_inv = zeros(8, 8);
coef_delta = zeros(8, 8);
coef_dering = zeros(8, 8);
var_dr_mtx = zeros(8, 8);
madi_dr_mtx = zeros(8, 8);

coef = T * (blk8 - 128) * T';

for row = 1:1
    for col = 1:4
        qtable_delta = zeros(8, 8);
        if col == 1
            qtable_delta(6, 3) = 1;
            qtable_delta(4, 4) = 1;
            qtable_delta(4, 3) = 1;
            qtable_delta(5, 4) = 1;
        elseif col == 2
            qtable_delta(6, 3) = 1;
            qtable_delta(4, 4) = 1;
            qtable_delta(5, 3) = 1;
            qtable_delta(4, 3) = 1;
        elseif col == 3
            qtable_delta(4, 2) = 1;
            qtable_delta(4, 4) = 1;
            qtable_delta(5, 3) = 1;
            qtable_delta(6, 1) = 1;
            qtable_delta(6, 3) = 1;
        else
            qtable_delta(4, 2) = 1;
            qtable_delta(4, 4) = 1;
            qtable_delta(5, 3) = 1;
            qtable_delta(6, 1) = 1;
            qtable_delta(6, 3) = 1;
            qtable_delta(4, 3) = 1;
        end
                
        for r = 1:8
            for c = 1:8
                coef_r(r, c) = round(coef(r, c) / qtable(r, c));
                coef_inv(r, c) = coef_r(r, c) * qtable(r, c);
%                 fprintf("%8d ", coef_inv(r, c));

                if coef_r(r, c) > 0
                    coef_dering(r, c) = (coef_r(r, c) - qtable_delta(r, c)) * qtable(r, c);
                    coef_delta(r, c) = qtable_delta(r, c) * qtable(r, c) * (-1);
                elseif coef_r(r, c) < 0
                    coef_dering(r, c) = (coef_r(r, c) + qtable_delta(r, c)) * qtable(r, c);
                    coef_delta(r, c) = qtable_delta(r, c) * qtable(r, c) * 1;
                end
            end
%             fprintf("\n");
        end

        recon_pix = round(T' * coef_inv * T + 128);
        recon_dering = round(T' * coef_dering * T + 128);
        recon_delta = round(T' * coef_delta * T);
        
        blk_sse = calc_sse(blk8, recon_dering, 8, 8);
        fprintf("blk_sse %d\n", blk_sse);

        [pix_ave, pix_var, pix_madi] = calc_var(recon_pix, 8, 8);

        [pix_ave_dr, pix_var_dr, pix_madi_dr] = calc_var(recon_dering, 8, 8);
        fprintf("delta(%d, %d) dering ave %d var %d madi %d\n", ...
                row, col, pix_ave_dr, pix_var_dr, pix_madi_dr);
            
        var_dr_mtx(row, col) = pix_var_dr;
        madi_dr_mtx(row, col) = pix_madi_dr;
    end
end

[min_v_col, row_arr] = min(var_dr_mtx);
[min_v_row, col_arr] = min(min_v_col);

fprintf("min_var_pos(%d, %d) %d\n", row_arr(col_arr), col_arr, min_v_row);

[min_v_col, row_arr] = min(madi_dr_mtx);
[min_v_row, col_arr] = min(min_v_col);

fprintf("min_madi_pos(%d, %d) %d\n", row_arr(col_arr), col_arr, min_v_row);

fprintf("\n");    

toc









