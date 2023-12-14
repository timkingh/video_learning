clear all
clc

width = 1280;
height = 720;

fid = fopen("F:\nfs\street_720p.yuv", "r");
fid_out = fopen("F:\nfs\street_720p_out_43.yuv", "w");

%y_mtx = fread(fid, [width, height]);
y_mtx = zeros(width, height);
uv_mtx = ones(height / 2, width) * 128;

out_mtx = y_mtx';


pix = 0;
pix_step = 1;
for m = 1:16:height
    for n = 1:16:width
        if m < 32 % hor
            for j = 0:15
                for k = 0:15
                    out_mtx(m + j, n + k) = pix;
                end

                if pix_step > 255
                    pix_step = 1;
                end

                pix = pix + pix_step;

                if pix > 255
                    pix = 0;
                    pix_step = pix_step + 1;
                end           
            end
        elseif m < 64 % ver
            if m == 33 && n == 1
                pix = 0;
                pix_step = 1;
            end
            
            for j = 0:15
                for k = 0:15
                    out_mtx(m + k, n + j) = pix;
                end
                
                if pix_step > 255
                    pix_step = 1;
                end

                pix = pix + pix_step;

                if pix > 255
                    pix = 0;
                    pix_step = pix_step + 1;
                end  
            end
        elseif m < 128
            val_sets = zeros(32, 1);
            if (m == 65 && n == 1) || (m == 97 && n == 1)
                pix = 0;
                pix_step = 1;
            end
            
            for kk = 1:31
                val_sets(kk, 1) = pix;
                
                if pix_step > 255
                    pix_step = 1;
                end
                
                pix = pix + pix_step;
                if pix > 255
                    pix = 0;
                    pix_step = pix_step + 1;
                end
            end
            
            for j = 0:15
                for k = 0:15
                    if m < 97
                        out_mtx(m + j, n + k) = val_sets(j + k + 1, 1);
                    else 
                        out_mtx(m + j, n + k) = val_sets(j - k + 16, 1);
                    end
                end
            end
        elseif m < 136
            for j = 0:7
                for k = 0:7
                    if rem(j, 2) == 0
                        out_mtx(m + j, n + k) = 255;
                    else
                        out_mtx(m + j, n + k) = 0;
                    end
                end
            end
        end
    end
end

count = fwrite(fid_out, out_mtx', 'uint8');
count = count + fwrite(fid_out, uv_mtx', 'uint8');
 
fclose(fid);
fclose(fid_out);
fprintf("fwirte %d bytes\n", count);
