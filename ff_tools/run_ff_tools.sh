#! /usr/bin/bash

IN_PATH=/mnt/udisk/light_change_yuv
OUT_PATH=/mnt/udisk/light_change_yuv/out

YUV_LIST="
test_0_indoor_alg_2304x1296_nv12
test_0_indoor_alg_night_2304x1296_nv12
test_1_outdoor_2304x1296_nv12
test_1_outdoor_night_2304x1296_nv12
test_2_outdoor_2304x1296_nv12
test_2_outdoor_night_2304x1296_nv12
test_3_front_desk_2304x1296_nv12
test_3_front_desk_night_2304x1296_nv12
test_4_front_desk_2304x1296_nv12
test_4_front_desk_night_2304x1296_nv12
test_5_darkroom_2304x1296_nv12
test_5_darkroom_night_2304x1296_nv12
test_6_darkroom_2304x1296_nv12
test_7_darkroom_2304x1296_nv12
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

draw_1920x1080_dspy