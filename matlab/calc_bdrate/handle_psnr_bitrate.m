function handle_psnr_bitrate(yuv_path, psnr_path, bitrate_path, result_path)

fid_yuv = fopen(yuv_path, 'rt');
psnr = load(psnr_path);
bitrate = load(bitrate_path);
bitrate = bitrate / 1000; % Kbps

yuv_list = textscan(fid_yuv, "%s\n");
%fprintf('%s\n', yuv_list{1}{1});
fclose(fid_yuv);

bd_all = 0;
nFileName = 1;
for i = 1:1:nFileName
    %w0
    SRC1 = [bitrate((i-1)*8 + 1), bitrate((i-1)*8 + 2), bitrate((i-1)*8 + 3), bitrate((i-1)*8 + 4)];
    PSNR_SRC1 = [psnr((i-1)*8 + 1), psnr((i-1)*8 + 2), psnr((i-1)*8 + 3), psnr((i-1)*8 + 4)];

    %w2
    SRC2 = [bitrate((i-1)*8 + 5), bitrate((i-1)*8 + 6), bitrate((i-1)*8 + 7), bitrate((i-1)*8 + 8)];
    PSNR_SRC2 = [psnr((i-1)*8 + 5), psnr((i-1)*8 + 6), psnr((i-1)*8 + 7), psnr((i-1)*8 + 8)];

    figure;
    hold on
    values = spcrv([[SRC1(1) SRC1 SRC1(end)];[PSNR_SRC1(1) PSNR_SRC1 PSNR_SRC1(end)]],3);
    plot(values(1,:),values(2,:), 'r');
    values = spcrv([[SRC2(1) SRC2 SRC2(end)];[PSNR_SRC2(1) PSNR_SRC2 PSNR_SRC2(end)]],3);
    plot(values(1,:),values(2,:), 'b');
    xlabel('Kbit rate')
    ylabel('PSNR(dB)')
    legend('org', 'new', 'Location','NorthWest')
    grid on
    x_min = floor(min([ min(SRC1); min(SRC2);]));
    x_max = ceil(max([ max(SRC1); max(SRC2);]));
    y_min = floor(min([ min(PSNR_SRC1); min(PSNR_SRC2);]));
    y_max = ceil(max([ max(PSNR_SRC1); max(PSNR_SRC2);]));
    x_stride = floor((x_max - x_min)/5);
    y_stride = floor((y_max - y_min)/5);
    set(gca, 'xtick', x_min:x_stride:x_max);
    set(gca, 'ytick', y_min:y_stride:y_max);
    title(strrep(yuv_list{1}{i},'_','\_'));

    mode = 'rate';
    bd = bjontegaard2(SRC1,PSNR_SRC1,SRC2,PSNR_SRC2, mode, result_path);
    bd_all = bd_all + bd;
    fprintf("%s bitrate difference %1.2f(%%)\n", yuv_list{1}{i}, bd - 100);
end

fprintf('Average bitrate difference between w0 and w2 over complete SNR range(BDBR) : %1.2f(%%)\n', bd_all / nFileName - 100);
end