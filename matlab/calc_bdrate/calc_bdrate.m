function calc_bdrate()

clear
close all;
clc;

root_path = ".\";
yuv_path = root_path + "yuv_list.txt";
psnr_path = root_path + "psnr_all.txt";
bitrate_path = root_path + "bitrate_all.txt";
result_path = root_path + "jpeg_bdrate.txt";
handle_psnr_bitrate(yuv_path, psnr_path, bitrate_path, result_path);

