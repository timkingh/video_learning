clear all
clc

width = 1280;
height = 720;


blk8 = [
   255   255   255   255   255   255   255   255
   255   255   255   255   255   255   255   255
   255   255   255   255   255   255   255   255
   255   255   255   255   255   255   255   255
   255   255   255   255   255   255   255   255
   255   255   255   255   255   255   255   255
   142   142   142   142   142   142   142   142
   128   128   128   128   128   128   128   128
];

fid = fopen("F:\nfs\street_720p_out_23.yuv", "r");
fid_out = fopen("F:\nfs\parse_log.txt", "w");

y_mtx = fread(fid, [width, height]);

out_mtx = y_mtx';

for k = 1:16:height
    for j = 1:16:width
        sum = 0;
        for m = 0:7
            for n = 0:7
                sum = sum + out_mtx(k + m, j + n);
            end
        end
        fprintf(fid_out, "pos(%d, %d) sum %d\n", j - 1, k - 1, sum);
        
        
        sum = 0;
        for m = 0:7
            for n = 0:7
                sum = sum + out_mtx(k + m, j + 8 + n);
            end
        end
        fprintf(fid_out, "pos(%d, %d) sum %d\n", j + 7, k - 1, sum);
        
        sum = 0;
        for m = 0:7
            for n = 0:7
                sum = sum + out_mtx(k + 8 + m, j + n);
            end
        end
        fprintf(fid_out, "pos(%d, %d) sum %d\n", j - 1, k + 7, sum);
        
        sum = 0;
        for m = 0:7
            for n = 0:7
                sum = sum + out_mtx(k + 8 + m, j + 8 + n);
            end
        end
        fprintf(fid_out, "pos(%d, %d) sum %d\n", j + 7, k + 7, sum);
    end
end
        
        


disp(sum);


% out_mtx(1:8, 1:8) = blk8;
% 
% blk8(8, 8) = 129;
% out_mtx(1:8, 9:16) = blk8;
% 
% blk8(8, 8) = 130;
% out_mtx(9:16, 1:8) = blk8;
% 
% blk8(8, 8) = 131;
% out_mtx(9:16, 9:16) = blk8;
% 
% blk8(8, 8) = 132;
% out_mtx(1:8, 17:24) = blk8;
% 
% blk8(8, 8) = 133;
% out_mtx(1:8, 25:32) = blk8;
% 
% blk8(8, 8) = 134;
% out_mtx(9:16, 17:24) = blk8;
% 
% blk8(8, 8) = 135;
% out_mtx(9:16, 25:32) = blk8;

% uv_mtx = randi([0, 255], height / 2, width);
% count = fwrite(fid_out, out_mtx', 'uint8');
% count = count + fwrite(fid_out, uv_mtx', 'uint8');
% fprintf("fwrite %d bytes\n", count);


fclose(fid);
fclose(fid_out);