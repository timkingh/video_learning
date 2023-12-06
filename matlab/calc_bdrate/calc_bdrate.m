function calc_bdrate()

clear
close all;
clc;

root_path = "me_20220119_hevc_s83\";
yuv_path = root_path + "yuv_list.txt";
psnr_path = root_path + "hevc_psnr.txt";
bitrate_path = root_path + "hevc_bitrate.txt";
result_path = root_path + "hevc_me_bdrate.txt";
handle_psnr_bitrate(yuv_path, psnr_path, bitrate_path, result_path);

