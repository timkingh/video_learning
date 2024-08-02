function calc_bdrate()

clear
close all;
clc;

root_path = ".\input_file\";
yuv_path = root_path + "yuv_list_subj.txt";
psnr_path = root_path + "hevc_psnr_rv1103b_test4.txt";
psnr_path2 = root_path + "hevc_psnr_rv1103b.txt";
bitrate_path = root_path + "hevc_filesize_rv1103b_test4.txt";
bitrate_path2 = root_path + "hevc_filesize_rv1103b.txt";
result_path = root_path + "bdrate.txt";
handle_psnr_bitrate_v2(yuv_path, psnr_path, bitrate_path, ...
                       psnr_path2, bitrate_path2, result_path);

