clear all
clc

fid = fopen('jpg_dec_rk_cities.txt', 'r');
fid_hisi = fopen('jpg_dec_hisi_cities.txt', 'r');

rk_coef = fscanf(fid, '%d');
hi_coef = fscanf(fid_hisi, '%d');
length = size(rk_coef);

for k = 1:length(1, 1)
   if abs(rk_coef(k, 1) - hi_coef(k, 1)) ~= 0
       fprintf("idx %d %d %d\n", k, rk_coef(k, 1), hi_coef(k, 1));
   end
end

fprintf("cmp %d coefs\n", k);

fclose(fid);
fclose(fid_hisi);