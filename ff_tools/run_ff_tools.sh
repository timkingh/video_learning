#! /usr/bin/bash

draw_960x540()
{
    ./ff_tools --input=/home/timkingh/yuv/vi_frame_960x540_1.yuv \
           --output=/home/timkingh/yuv/out_1080p.yuv \
           --width=960 --height=540 --disp_flg=1 \
           --out_scale=2 --madi_thd=1 --madp_thd=1 \
           --input_fmt=1 --frames=32
}

draw_1920x1080()
{
    ./ff_tools --input=/home/timkingh/yuv/genshin_impact_0_1080p.yuv \
           --output=/home/timkingh/yuv/genshin_impact_0_4k.yuv \
           --width=1920 --height=1080 --disp_flg=1 \
           --out_scale=2 --madi_thd=0 --madp_thd=1 \
           --input_fmt=0 --frames=3
}

draw_1920x1080