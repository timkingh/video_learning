#! /usr/bin/bash

IN_PATH=/mnt/udisk/hikvision/yuv_smart_v3/
OUT_PATH=/mnt/udisk/hikvision/yuv_smart_v3/output

YUV_LIST="
A_1080p_3000frames
B_1080p_3000frames
C_1080p_3000frames
D_1080p_3000frames
E_1080p_3000frames
"

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

draw_1920x1080_dspy()
{
    for yuv in $YUV_LIST; do
        ./ff_tools --input=$IN_PATH/$yuv.yuv \
                --output=$OUT_PATH/${yuv}_dspy.yuv \
                --width=2304 --height=1296 --disp_flg=3 \
                --out_scale=1 --madi_thd=0 --madp_thd=1 \
                --input_fmt=1 --frames=3000
    done
}

draw_1920x1080_nn_results()
{
    for yuv in $YUV_LIST; do
        ./ff_tools --input=$IN_PATH/$yuv.yuv \
                --output=$OUT_PATH/${yuv}_nn_results.yuv \
                --nn_results=$OUT_PATH/nn_out_${yuv}.txt \
                --width=1920 --height=1080 --disp_flg=3 \
                --out_scale=1 --aligned_size=16 \
                --input_fmt=0 --frames=300
    done
}

#draw_1920x1080_dspy
draw_1920x1080_nn_results