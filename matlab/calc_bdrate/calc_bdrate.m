function calc_bdrate()

clear
close all;
clc;

root_path = ".\";
yuv_path = root_path + "yuv_list.txt";
psnr_path = root_path + "psnr_all.txt";
psnr_path2 = root_path + "psnr_rk.txt";
bitrate_path = root_path + "bitrate_all.txt";
bitrate_path2 = root_path + "bitrate_rk.txt";
result_path = root_path + "jpeg_bdrate.txt";
handle_psnr_bitrate_v2(yuv_path, psnr_path, bitrate_path, ...
                       psnr_path2, bitrate_path2, result_path);

