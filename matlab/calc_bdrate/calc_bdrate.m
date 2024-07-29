function calc_bdrate()

clear
close all;
clc;

root_path = ".\input_file\";
yuv_path = root_path + "yuv_list_subj.txt";
psnr_path = root_path + "avc_psnr_rv1106_test1.txt";
psnr_path2 = root_path + "avc_psnr_rv1106.txt";
bitrate_path = root_path + "avc_filesize_rv1106_test1.txt";
bitrate_path2 = root_path + "avc_filesize_rv1106.txt";
result_path = root_path + "bdrate.txt";
handle_psnr_bitrate_v2(yuv_path, psnr_path, bitrate_path, ...
                       psnr_path2, bitrate_path2, result_path);

