#!/usr/bin/bash

INPUT_PATH=/mnt/winshare/av1_stream

INPUT_FILE="
REC000T-20240529-174544-174844
REC000T-20240529-174844-174844
"

for filename in $INPUT_FILE
do
     echo "$filename"
    ./VideoCalc -i=$INPUT_PATH/$filename.avi -o=$INPUT_PATH/$filename.av1 \
                -w=2560 -h=1440 -m=6 -f=500000 --fps=15
    #ffmpeg -hide_banner -i $INPUT_PATH/$filename.av1 -f null -
done