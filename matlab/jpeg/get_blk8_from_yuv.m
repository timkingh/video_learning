function blk8 = get_blk8_from_yuv(pic, width, height, blk8_idx)

blk8 = zeros(8, 8);
blk8_cnt = 0;

for m = 1:16:height
    for n = 1:16:width
        if blk8_cnt == blk8_idx
            blk8 = pic((m+0):(m+7), (n+0):(n+7));
            return;
        end
        
        blk8_cnt = blk8_cnt + 1;
        if blk8_cnt == blk8_idx
            blk8 = pic((m+0):(m+7), (n+8):(n+15));
            return;
        end
        
        
        blk8_cnt = blk8_cnt + 1;
        if blk8_cnt == blk8_idx
            blk8 =  pic((m+8):(m+15), (n+0):(n+7));
            return;
        end
       
        blk8_cnt = blk8_cnt + 1;
        if blk8_cnt == blk8_idx
            blk8 = pic((m+8):(m+15), (n+8):(n+15));
            return;
        end
        blk8_cnt = blk8_cnt + 1;
    end
end