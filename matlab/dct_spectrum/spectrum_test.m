clear all
%clc
tic

fid_out = fopen("D:\v2_rkvenc\jpeg\20231201\log.txt", "w");
T = dctmtx(8);
min_v = 0;
max_v = 255;
value = [ min_v, max_v ];
iter_num = 200000000;
target_coef = 0;

coef_min_max = ones(5, 2);
coef_min_max(:, 1) = coef_min_max(:, 1) * 2048;
coef_min_max(:, 2) = coef_min_max(:, 2) * (-2048);


% order(4): org_min, org_max, coef_min, coef_max
% pos(5): (1, 1), (3, 4), (4, 1), (6, 3), (8, 8)
ret_mtx = zeros(8, 8, 20); % 4 * 5
coord = [ 1, 1;
          3, 4;
          4, 1;
          6, 3;
          8, 8;
        ];

running_rate = zeros(1, 8);
for m = 1:8
    running_rate(1, m) = iter_num * m / 8;
end

for m = 1:iter_num
    blk8 = [ value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
             value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2)) value(1, randi(2))
            ];

    coef = T * (blk8 - 128) * T';
    coef = round(coef);
    
    for n = 1:5
        target_coef = coef(coord(n, 1), coord(n, 2));
        
        if target_coef < coef_min_max(n, 1)
            coef_min_max(n, 1) = target_coef;
            ret_mtx(:, :, (n - 1) * 4 + 1) = blk8;
            ret_mtx(:, :, (n - 1) * 4 + 3) = coef;
        end
        
        if target_coef > coef_min_max(n, 2)
            coef_min_max(n, 2) = target_coef;
            ret_mtx(:, :, (n - 1) * 4 + 2) = blk8;
            ret_mtx(:, :, (n - 1) * 4 + 4) = coef;
        end
    end
    
    for hh = 1:8
        if m == running_rate(1, hh)
            fprintf("running %f\n", m / iter_num);
            break;
        end
    end
end

str_coord = [ "(1, 1)", "(3, 4)", "(4, 1)", "(6, 3)", "(8, 8)" ];
log_str = [ "org_min", "org_max", "coef_min", "coef_max" ];
for j = 1:5
    
    fprintf("iter_num %d pos%s coef_min %d coef_max %d\n", ...
            iter_num, str_coord(1, j), coef_min_max(j, 1), coef_min_max(j, 2));
    fprintf(fid_out, "iter_num %d pos%s coef_min %d coef_max %d\n", ...
            iter_num, str_coord(1, j), coef_min_max(j, 1), coef_min_max(j, 2));

    for k = 1:4
        fprintf(fid_out, "%s\n", log_str(1, k));
        for m = 1:8
            for n = 1:8
                fprintf(fid_out, "%5d ", ret_mtx(m, n, (j - 1) * 4 + k));
            end

            fprintf(fid_out, "\n");
        end
        fprintf(fid_out, "\n");
    end
end

fclose(fid_out);

fprintf("finished!\n");
toc

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




