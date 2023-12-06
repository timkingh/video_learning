function bd_psnr()
clear
close all;
clc;


psnr = load('psnr.txt');
bitrate = load('bitrate.txt');

bd_all = 0;
nFileName = 1;
for i = 1:1:nFileName
    %rk1109
    SRC1 = [bitrate((i-1)*8 + 1), bitrate((i-1)*8 + 2), bitrate((i-1)*8 + 3), bitrate((i-1)*8 + 4)];
    PSNR_SRC1 = [psnr((i-1)*8 + 1), psnr((i-1)*8 + 2), psnr((i-1)*8 + 3), psnr((i-1)*8 + 4)];

    %hi3519v101
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
    legend('rk3328 h265', 'rk1109 h265', 'Location','NorthWest')
    grid on
    x_min = floor(min([ min(SRC1); min(SRC2);]));
    x_max = ceil(max([ max(SRC1); max(SRC2);]));
    y_min = floor(min([ min(PSNR_SRC1); min(PSNR_SRC2);]));
    y_max = ceil(max([ max(PSNR_SRC1); max(PSNR_SRC2);]));
    x_stride = floor((x_max - x_min)/5);
    y_stride = floor((y_max - y_min)/5);
    set(gca, 'xtick', x_min:x_stride:x_max);
    set(gca, 'ytick', y_min:y_stride:y_max);
    fileName = {
    'Trial4\_Qv2\_Sc3\_Hv1SideEnt\_tae'
    'Trial15\_Qv2\_Sc2\_Fremont\_Backyard'
    'Qv2\_HDR\_30Water\_30Foliage\_1PersonPacing'
    'HV3\_KitchenEntrance\_FarCloseActivities\_BisExStrong\_7574.trim'
    'jets\_720p30'
    'cyclists\_720p30'
    'slideediting\_720p30'
    'Kimono1\_1920x1080\_24'
    'PeopleOnStreet\_2560x1600\_30\_crop'
    'riverbed\_1080p\_25'};
    title(fileName(i));

    mode = 'rate';
    bd = bjontegaard2(SRC1,PSNR_SRC1,SRC2,PSNR_SRC2, mode);
    bd_all = bd_all + bd;
end

str2_1 = sprintf('Average bitrate difference between VP9 and AV1 over complete SNR range(BDBR)  : %1.2f(%%)', bd_all / nFileName);
disp(str2_1);
end