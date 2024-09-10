#! /usr/bin/bash

draw_960x540()
{
    ./ff_tools --input=/home/timkingh/yuv/vi_frame_960x540_1.yuv \
           --output=/home/timkingh/yuv/out_1080p.yuv \
           --width=960 --height=540 --disp_flg=2 \
           --out_scale=2 --madi_thd=1 --madp_thd=1 \
           --input_fmt=1 --frames=32
}

draw_1920x1080()
{
    ./ff_tools --input=/home/timkingh/yuv/311_1080p_10000kbps_h265_300frames.yuv \
           --output=/home/timkingh/yuv/out_4k.yuv \
           --width=1920 --height=1080 --disp_flg=2 \
           --out_scale=2 --madi_thd=1 --madp_thd=1 \
           --input_fmt=0 --frames=3
}

draw_960x540