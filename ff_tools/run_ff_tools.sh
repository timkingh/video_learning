#! /usr/bin/bash

IN_PATH=/mnt/udisk/oqe
OUT_PATH=/mnt/udisk/light_change_yuv

YUV_LIST="
31002_1080p_10000kbps_h265_300frames
31705_1080p_10000kbps_h265_300frames
31901_1080p_10000kbps_h265_300frames
31902_1080p_10000kbps_h265_300frames
dahua_ae_adjust_1080p_yuv420p
tplink_1080p_yuv420p_119frames_test23_ipc
tplink_1080p_yuv420p_160frames_test11_ipc
tplink_1080p_yuv420p_160frames_test12_ipc
tplink_1080p_yuv420p_160frames_test13_ipc
tplink_1080p_yuv420p_160frames_test14_ipc
tplink_1080p_yuv420p_160frames_test15_ipc
tplink_1080p_yuv420p_160frames_test16_ipc
tplink_1080p_yuv420p_160frames_test17_ipc
tplink_1080p_yuv420p_160frames_test18_ipc
tplink_1080p_yuv420p_160frames_test19_ipc
tplink_1080p_yuv420p_160frames_test20_ipc
tplink_1080p_yuv420p_160frames_test21_ipc
tplink_1080p_yuv420p_160frames_test22_ipc
tplink_1080p_yuv420p_160frames_test2_ipc
tplink_1080p_yuv420p_160frames_test4_ipc
tplink_1080p_yuv420p_160frames_test5_ipc
tplink_1080p_yuv420p_160frames_test6_ipc
tplink_1080p_yuv420p_160frames_test8_ipc
tplink_1080p_yuv420p_200frames_test4_ircut_ipc
tplink_1080p_yuv420p_203frames_test2_ircut_ipc
tplink_1080p_yuv420p_227frames_test0_ircut_ipc
tplink_1080p_yuv420p_250frames_test1_ircut_ipc
tplink_1080p_yuv420p_250frames_test3_ircut_ipc
tplink_1080p_yuv420p_test0
tplink_1080p_yuv420p_test1
tplink_1080p_yuv420p_test10
tplink_1080p_yuv420p_test11
tplink_1080p_yuv420p_test12
tplink_1080p_yuv420p_test13
tplink_1080p_yuv420p_test14
tplink_1080p_yuv420p_test15
tplink_1080p_yuv420p_test16
tplink_1080p_yuv420p_test17
tplink_1080p_yuv420p_test18
tplink_1080p_yuv420p_test19
tplink_1080p_yuv420p_test2
tplink_1080p_yuv420p_test20
tplink_1080p_yuv420p_test21
tplink_1080p_yuv420p_test22
tplink_1080p_yuv420p_test23
tplink_1080p_yuv420p_test3
tplink_1080p_yuv420p_test4
tplink_1080p_yuv420p_test5
tplink_1080p_yuv420p_test6
tplink_1080p_yuv420p_test7
tplink_1080p_yuv420p_test8
tplink_1080p_yuv420p_test9_0
tplink_1080p_yuv420p_test9_1
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
                --width=1920 --height=1080 --disp_flg=3 \
                --out_scale=1 --madi_thd=0 --madp_thd=1 \
                --input_fmt=0 --frames=300
    done
}

draw_1920x1080_dspy