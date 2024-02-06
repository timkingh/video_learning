clear all
clc

width = 1920;
height = 1080;
blk8_num = width / 8 * height / 8;
blk8_dr0 = zeros(8, 8);
blk8_dr1 = zeros(8, 8);
qtable_delta = [
     0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0
     0 1 0 1 0 0 0 0
     0 0 1 0 0 0 0 0
     1 0 1 0 0 0 0 0
     0 0 0 0 0 0 0 0
     0 0 0 0 0 0 0 0
];

for qf = 1:99
    file_path = "D:\\code\\video_learning\\matlab\\dering_test\\input_file\\";
    file_path0 = file_path + "jpg_dec_street_1080p_part_hisi_r73_qf%d_dering0.txt";
    file_name = sprintf(file_path0, qf);
    [coef0, pos0] = open_jpeg_coef_file(file_name, width, height);

    file_path1 = file_path + "jpg_dec_street_1080p_part_hisi_r73_qf%d_dering1.txt";
    file_name = sprintf(file_path1, qf);
    [coef1, pos1] = open_jpeg_coef_file(file_name, width, height);
    fprintf("qf %d\n", qf);
    
    for iter = 1:blk8_num
        blk8_dr0 = coef0(:, :, iter);
        blk8_dr1 = coef1(:, :, iter);
        pos_x = pos0(iter, 1);
        pos_y = pos0(iter, 2);

        coef_delta = zeros(8, 8);
        for r = 1:8
            for c = 1:8
                if blk8_dr0(r, c) ~= blk8_dr1(r, c)
                    coef_delta(r, c) = 1;
                end
            end
        end

        if isequal(qtable_delta, coef_delta)
            fprintf("qf %d pos(%d, %d)\n", qf, pos_x, pos_y);
        end
    end
end
