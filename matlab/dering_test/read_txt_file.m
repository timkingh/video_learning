clear all
clc
tic

width = 1280;
height = 720;

fid_dering0 = fopen('jpg_dec_street_720p_hisi_r23_q90_dering0_only.txt', 'r');
fid_dering1 = fopen('jpg_dec_street_720p_hisi_r23_q90_dering1_only.txt', 'r');
fid_var = fopen('out_var_street_720p.txt', 'r');
fid_out = fopen('dec_cmp_out.txt', 'w');
fid_out_0 = fopen('dering0.txt', 'w');
fid_out_1 = fopen('dering1.txt', 'w');


blk_num = 0;
out_mtx = zeros(8, 8);

for row = 1:16:height
    for col = 1:16:width
        for idx = 0:3
            pos_x = col - 1 + rem(idx, 2) * 8;
            pos_y = row - 1 + floor(idx / 2) * 8;
            coef0 = fscanf(fid_dering0, '%d', [8, 8]);
            coef1 = fscanf(fid_dering1, '%d', [8, 8]);
            dir = fscanf(fid_var, 'pos(%d, %d) dir %d var %d\n', [4, 1]);
            if isequal(coef0, coef1) ~= 1
                 fprintf(fid_out, "frame=0, cu_x=%d, cu_y=%d, cu_size=8, mv_x=0, mv_y=-2\n", ...
                         pos_x / 8, pos_y / 8);
                 blk_num = blk_num + 1;
            end
%             if dir(3, 1) == 2 && dir(4, 1) > 256
%                 fprintf(fid_out_0, "pos(%d, %d) dir %d var %d\n", ...
%                         dir(1, 1), dir(2, 1), dir(3, 1), dir(4, 1));
%                 fprintf(fid_out_1, "pos(%d, %d) dir %d var %d\n", ...
%                         dir(1, 1), dir(2, 1), dir(3, 1), dir(4, 1));
%                 for m = 1:8
%                     for n = 1:8
%                         fprintf(fid_out_0, "%4d ", coef0(n, m));
%                         fprintf(fid_out_1, "%4d ", coef1(n, m));
%                     end
%                     fprintf(fid_out_0, "\n\n");
%                     fprintf(fid_out_1, "\n\n");
%                 end
%                 fprintf(fid_out_0, "\n");
%                 fprintf(fid_out_1, "\n");
%                 blk_num = blk_num + 1;
%             end
        end
    end
end


fclose(fid_dering0);
fclose(fid_dering1);
fclose(fid_var);
fclose(fid_out);
fclose(fid_out_0);
fclose(fid_out_1);


fprintf("blk_num %d\n", blk_num);
toc