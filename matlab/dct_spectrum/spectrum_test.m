clear all
clc

fid_out = fopen("D:\v2_rkvenc\jpeg\20231201\log.txt", "w");
T = dctmtx(8);
min_v = 0;


for max_v = 0:15:255
    blk8 = [   min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
               min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
               min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
               min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
               min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
               min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
               min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
               min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
            ];

    coef = T * (blk8 - 128) * T';

    fprintf(fid_out, "min:max %d:%d\n", min_v, max_v);
    for m = 1:8
        for n = 1:8
            fprintf(fid_out, "%5d ", round(coef(m, n)));
        end

        fprintf(fid_out, "\n");
    end
    fprintf(fid_out, "\n");
end


fclose(fid_out);

fprintf("finished!\n");

% 01010101
%     blk8 = [   min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
%                max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
%                min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
%                max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
%                min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
%                max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
%                min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
%                max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
%             ];

% 00110011
%     blk8 = [   min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
%                min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
%                max_v   max_v   min_v   min_v   max_v   max_v   min_v   min_v
%                max_v   max_v   min_v   min_v   max_v   max_v   min_v   min_v
%                min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
%                min_v   min_v   max_v   max_v   min_v   min_v   max_v   max_v
%                max_v   max_v   min_v   min_v   max_v   max_v   min_v   min_v
%                max_v   max_v   min_v   min_v   max_v   max_v   min_v   min_v
%             ];

% 00001111
%     blk8 = [   min_v   min_v   min_v   min_v   max_v   max_v   max_v   max_v
%                min_v   min_v   min_v   min_v   max_v   max_v   max_v   max_v
%                min_v   min_v   min_v   min_v   max_v   max_v   max_v   max_v
%                min_v   min_v   min_v   min_v   max_v   max_v   max_v   max_v
%                max_v   max_v   max_v   max_v   min_v   min_v   min_v   min_v
%                max_v   max_v   max_v   max_v   min_v   min_v   min_v   min_v
%                max_v   max_v   max_v   max_v   min_v   min_v   min_v   min_v
%                max_v   max_v   max_v   max_v   min_v   min_v   min_v   min_v
%             ];




