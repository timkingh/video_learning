clear all
clc

width = 1280;
height = 720;

fid = fopen("F:\nfs\street_720p.yuv", "r");
fid_out = fopen("F:\nfs\street_720p_out_28.yuv", "w");

y_mtx = fread(fid, [width, height]);
uv_mtx = ones(height / 2, width) * 128;

out_mtx = y_mtx';


pix = 0;
pix_step = 1;
for m = 1:16:height
    for n = 1:16:width
        for j = 0:15
            for k = 0:15
                out_mtx(m + j, n + k) = pix;
            end
            
            pix = pix + pix_step;
            
            if pix > 255
                pix = 0;
                pix_step = pix_step + 1;
            end
        end
    end
end

count = fwrite(fid_out, out_mtx', 'uint8');
count = count + fwrite(fid_out, uv_mtx', 'uint8');
 
fclose(fid);
fclose(fid_out);
fprintf("fwirte %d bytes\n", count);
