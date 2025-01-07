#! /usr/bin/bash

IN_PATH=/mnt/udisk/tongli
OUT_PATH=/mnt/udisk/tongli/out

YUV_LIST="
T8418N_05_2304x1296
T8418N_10_2304x1296
T8418N_15_2304x1296
T8418N_20_2304x1296
T8418N_25_2304x1296
T8418N_30_2304x1296
T8418N_35_2304x1296
T8418N_40_2304x1296
T8418N_44_2304x1296
T8418N_50_2304x1296
T8418N_55_2304x1296
T8418N_60_2304x1296
T8418N_65_2304x1296
T8418N_70_2304x1296
T8418N_74_2304x1296
T8418N_79_2304x1296
T8418N_83_2304x1296
T8418N_88_2304x1296
T8418N_89_2304x1296
T8W11C_05_2304x1296
T8W11C_10_2304x1296
T8W11C_15_2304x1296
T8W11C_20_2304x1296
T8W11C_25_2304x1296
T8W11C_30_2304x1296
T8W11C_35_2304x1296
T8W11C_40_2304x1296
T8W11C_44_2304x1296
T8W11C_50_2304x1296
T8W11C_55_2304x1296
T8W11C_60_2304x1296
T8W11C_65_2304x1296
T8W11C_70_2304x1296
T8W11C_71_2304x1296
T8W11C_76_2304x1296
T8W11C_80_2304x1296
T8W11C_85_2304x1296
T8W11C_86_2304x1296
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
                --input_fmt=0 --frames=3000
    done
}

draw_1920x1080_dspy