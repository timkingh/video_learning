clear all
clc

width = 1280;
height = 720;

fid_dering1 = fopen('jpg_dec_street_hisi_r7_m3_deringlvl1_dct_only.txt', 'r');
fid_dering0 = fopen('jpg_dec_street_hisi_r7_m3_deringlvl0.txt', 'r');


for k = 1:4
    coef0 = fscanf(fid_dering0, '%d', [8, 8]);
    coef1 = fscanf(fid_dering1, '%d', [8, 8]);
    if coef0 == coef1
        fprintf("same matrix!\n");
    end
end


fclose(fid_dering0);
fclose(fid_dering1);