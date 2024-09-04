#!/usr/bin/bash

INPUT_PATH=/mnt/shared/av1/2024-05-30

INPUT_FILE="
REC000T-20240530-000300-000600
REC000T-20240530-000600-000900
REC000T-20240530-000900-001200
REC000T-20240530-001200-001500
REC000T-20240530-001500-001800
REC000T-20240530-001800-002100
REC000T-20240530-002100-002400
REC000T-20240530-002400-002700
REC000T-20240530-002700-003000
REC000T-20240530-003000-003300
REC000T-20240530-003300-003600
REC000T-20240530-003600-003900
REC000T-20240530-003900-004200
REC000T-20240530-004200-004500
REC000T-20240530-004500-004800
REC000T-20240530-004800-005100
REC000T-20240530-005100-005400
REC000T-20240530-005400-005700
REC000T-20240530-005700-010000
REC000T-20240530-010000-010300
REC000T-20240530-010300-010600
REC000T-20240530-010600-010900
REC000T-20240530-010900-011200
REC000T-20240530-011200-011500
REC000T-20240530-011500-011800
REC000T-20240530-011800-012100
REC000T-20240530-012100-012400
REC000T-20240530-012400-012700
REC000T-20240530-012700-013000
REC000T-20240530-013000-013300
REC000T-20240530-013300-013600
REC000T-20240530-013600-013900
REC000T-20240530-013900-014200
REC000T-20240530-014200-014500
REC000T-20240530-014500-014800
REC000T-20240530-014800-015100
REC000T-20240530-015100-015400
REC000T-20240530-015400-015700
REC000T-20240530-015700-020000
REC000T-20240530-020000-020300
REC000T-20240530-020300-020600
REC000T-20240530-020600-020900
REC000T-20240530-020900-021200
REC000T-20240530-021200-021500
REC000T-20240530-021500-021800
REC000T-20240530-021800-022100
REC000T-20240530-022100-022400
REC000T-20240530-022400-022700
REC000T-20240530-022700-023000
REC000T-20240530-023000-023300
REC000T-20240530-023300-023600
REC000T-20240530-023600-023900
REC000T-20240530-023900-024200
REC000T-20240530-024200-024500
REC000T-20240530-024500-024800
REC000T-20240530-024800-025100
REC000T-20240530-025100-025400
REC000T-20240530-025400-025700
REC000T-20240530-025700-030000
REC000T-20240530-030000-030300
REC000T-20240530-030300-030600
REC000T-20240530-030600-030900
REC000T-20240530-030900-031200
REC000T-20240530-031200-031500
REC000T-20240530-031500-031800
REC000T-20240530-031800-032100
REC000T-20240530-032100-032400
REC000T-20240530-032400-032700
REC000T-20240530-032700-033000
REC000T-20240530-033000-033300
REC000T-20240530-033300-033600
REC000T-20240530-033600-033900
REC000T-20240530-033900-034200
REC000T-20240530-034200-034500
REC000T-20240530-034500-034800
REC000T-20240530-034800-035100
REC000T-20240530-035100-035400
REC000T-20240530-035400-035700
REC000T-20240530-035700-040000
REC000T-20240530-040000-040300
REC000T-20240530-040300-040600
REC000T-20240530-040600-040900
REC000T-20240530-040900-041200
REC000T-20240530-041200-041500
REC000T-20240530-041500-041800
REC000T-20240530-041800-042100
REC000T-20240530-042100-042400
REC000T-20240530-042400-042700
REC000T-20240530-042700-043000
REC000T-20240530-043000-043300
REC000T-20240530-043300-043600
REC000T-20240530-043600-043900
REC000T-20240530-043900-044200
REC000T-20240530-044200-044500
REC000T-20240530-044500-044800
REC000T-20240530-044800-045100
REC000T-20240530-045100-045400
REC000T-20240530-045400-045700
REC000T-20240530-045700-050000
REC000T-20240530-050000-050300
REC000T-20240530-050300-050600
REC000T-20240530-050600-050900
REC000T-20240530-050900-051200
REC000T-20240530-051200-051500
REC000T-20240530-051500-051800
REC000T-20240530-051800-052100
REC000T-20240530-052100-052400
REC000T-20240530-052400-052700
REC000T-20240530-052700-053000
REC000T-20240530-053000-053300
REC000T-20240530-053300-053600
REC000T-20240530-053600-053900
REC000T-20240530-053900-054200
REC000T-20240530-054200-054500
REC000T-20240530-054500-054800
REC000T-20240530-054800-055100
REC000T-20240530-055100-055400
REC000T-20240530-055400-055700
REC000T-20240530-055700-060000
REC000T-20240530-060000-060300
REC000T-20240530-060300-060600
REC000T-20240530-060600-060900
REC000T-20240530-060900-061200
REC000T-20240530-061200-061500
REC000T-20240530-061500-061800
REC000T-20240530-061800-062100
REC000T-20240530-062100-062400
REC000T-20240530-062400-062700
REC000T-20240530-062700-063000
REC000T-20240530-063000-063300
REC000T-20240530-063300-063600
REC000T-20240530-063600-063900
REC000T-20240530-063900-064200
REC000T-20240530-064200-064500
REC000T-20240530-064500-064800
REC000T-20240530-064800-065100
REC000T-20240530-065100-065400
REC000T-20240530-065400-065700
REC000T-20240530-065700-070000
REC000T-20240530-070000-070300
REC000T-20240530-070300-070600
REC000T-20240530-070600-070900
REC000T-20240530-070900-071200
REC000T-20240530-071200-071500
REC000T-20240530-071500-071800
REC000T-20240530-071800-072100
REC000T-20240530-072100-072400
REC000T-20240530-072400-072700
REC000T-20240530-072700-073000
REC000T-20240530-073000-073300
REC000T-20240530-073300-073600
REC000T-20240530-073600-073900
REC000T-20240530-073900-074200
REC000T-20240530-074200-074500
REC000T-20240530-074500-074800
REC000T-20240530-074800-075100
REC000T-20240530-075100-075400
REC000T-20240530-075400-075700
REC000T-20240530-075700-080000
REC000T-20240530-080000-080300
REC000T-20240530-080300-080600
REC000T-20240530-080600-080900
REC000T-20240530-080900-081200
REC000T-20240530-081200-081500
REC000T-20240530-081500-081800
REC000T-20240530-081800-082100
REC000T-20240530-082100-082400
REC000T-20240530-082400-082700
REC000T-20240530-082700-083000
REC000T-20240530-083000-083300
REC000T-20240530-083300-083600
REC000T-20240530-083600-083900
REC000T-20240530-083900-084200
REC000T-20240530-084200-084500
REC000T-20240530-084500-084800
REC000T-20240530-084800-085100
REC000T-20240530-085100-085400
REC000T-20240530-085400-085700
REC000T-20240530-085700-090000
REC000T-20240530-090000-090300
REC000T-20240530-090300-090600
REC000T-20240530-090600-090900
REC000T-20240530-090900-091200
REC000T-20240530-091200-091500
REC000T-20240530-091500-091800
REC000T-20240530-091800-092100
REC000T-20240530-092100-092400
REC000T-20240530-092400-092700
REC000T-20240530-092700-093000
REC000T-20240530-093000-093300
REC000T-20240530-093300-093600
REC000T-20240530-093600-093900
REC000T-20240530-093900-094200
REC000T-20240530-094200-094500
REC000T-20240530-094500-094800
REC000T-20240530-094800-095100
REC000T-20240530-095100-095400
REC000T-20240530-095400-095700
REC000T-20240530-095700-100000
REC000T-20240530-100000-100300
REC000T-20240530-100300-100600
REC000T-20240530-100600-100900
REC000T-20240530-100900-101200
REC000T-20240530-101200-101500
REC000T-20240530-101500-101800
REC000T-20240530-101800-102100
REC000T-20240530-102100-102400
REC000T-20240530-102400-102700
REC000T-20240530-102700-103000
REC000T-20240530-103000-103300
REC000T-20240530-103300-103600
REC000T-20240530-103600-103900
REC000T-20240530-103900-104200
REC000T-20240530-104200-104500
REC000T-20240530-104500-104800
REC000T-20240530-104800-105100
REC000T-20240530-105100-105400
REC000T-20240530-105400-105700
REC000T-20240530-105700-110000
REC000T-20240530-110000-110300
REC000T-20240530-110300-110600
REC000T-20240530-110600-110900
REC000T-20240530-110900-111200
REC000T-20240530-111200-111500
REC000T-20240530-111500-111800
REC000T-20240530-111800-112100
REC000T-20240530-112100-112400
REC000T-20240530-112400-112700
REC000T-20240530-112700-113000
REC000T-20240530-113000-113300
REC000T-20240530-113300-113600
REC000T-20240530-113600-113900
REC000T-20240530-113900-114200
REC000T-20240530-114200-114500
REC000T-20240530-114500-114800
REC000T-20240530-114800-115100
REC000T-20240530-115100-115400
REC000T-20240530-115400-115700
REC000T-20240530-115700-120000
REC000T-20240530-120000-120300
REC000T-20240530-120300-120600
REC000T-20240530-120600-120900
REC000T-20240530-120900-121200
REC000T-20240530-121200-121500
REC000T-20240530-121500-121800
REC000T-20240530-121800-122100
REC000T-20240530-122100-122400
REC000T-20240530-122400-122700
REC000T-20240530-122700-123000
REC000T-20240530-123000-123300
REC000T-20240530-123300-123600
REC000T-20240530-123600-123900
REC000T-20240530-123900-124200
REC000T-20240530-124200-124500
REC000T-20240530-124500-124800
REC000T-20240530-124800-125100
REC000T-20240530-125100-125400
REC000T-20240530-125400-125700
REC000T-20240530-125700-130000
REC000T-20240530-130000-130300
REC000T-20240530-130300-130600
REC000T-20240530-130600-130900
REC000T-20240530-130900-131200
REC000T-20240530-131200-131500
REC000T-20240530-131500-131800
REC000T-20240530-131800-132100
REC000T-20240530-132100-132400
REC000T-20240530-132400-132700
REC000T-20240530-132700-133000
REC000T-20240530-133000-133300
REC000T-20240530-133300-133600
REC000T-20240530-133600-133900
REC000T-20240530-133900-134200
REC000T-20240530-134200-134500
REC000T-20240530-134500-134800
REC000T-20240530-134800-135100
REC000T-20240530-135100-135400
REC000T-20240530-135400-135700
REC000T-20240530-135700-140000
REC000T-20240530-140000-140300
REC000T-20240530-140300-140600
REC000T-20240530-140600-140900
REC000T-20240530-140900-141200
REC000T-20240530-141200-141500
REC000T-20240530-141500-141800
REC000T-20240530-141800-142100
REC000T-20240530-142100-142400
REC000T-20240530-142400-142700
REC000T-20240530-142700-143000
REC000T-20240530-143000-143300
REC000T-20240530-143300-143600
REC000T-20240530-143600-143900
REC000T-20240530-143900-144200
REC000T-20240530-144200-144500
REC000T-20240530-144500-144800
REC000T-20240530-144800-145100
REC000T-20240530-145100-145400
REC000T-20240530-145400-145700
REC000T-20240530-145700-150000
REC000T-20240530-150000-150300
REC000T-20240530-150300-150600
REC000T-20240530-150600-150900
REC000T-20240530-150900-151200
REC000T-20240530-151200-151500
REC000T-20240530-151500-151800
REC000T-20240530-151800-152100
REC000T-20240530-152100-152400
REC000T-20240530-152400-152700
REC000T-20240530-152700-153000
REC000T-20240530-153000-153300
REC000T-20240530-153300-153600
REC000T-20240530-153600-153900
REC000T-20240530-153900-154200
REC000T-20240530-154200-154500
REC000T-20240530-154500-154800
REC000T-20240530-154800-155100
REC000T-20240530-155100-155400
REC000T-20240530-155400-155700
REC000T-20240530-155700-160000
REC000T-20240530-160000-160300
REC000T-20240530-160300-160600
REC000T-20240530-160600-160900
REC000T-20240530-160900-161200
REC000T-20240530-161200-161500
REC000T-20240530-161500-161800
REC000T-20240530-161800-162100
REC000T-20240530-162100-162400
REC000T-20240530-162400-162700
REC000T-20240530-162700-163000
REC000T-20240530-163000-163300
REC000T-20240530-163300-163600
REC000T-20240530-163600-163900
REC000T-20240530-163900-164200
REC000T-20240530-164200-164500
REC000T-20240530-164500-164800
REC000T-20240530-164800-165100
REC000T-20240530-165100-165400
REC000T-20240530-165400-165700
REC000T-20240530-165700-170000
REC000T-20240530-170000-170300
REC000T-20240530-170300-170600
REC000T-20240530-170600-170900
REC000T-20240530-170900-171200
REC000T-20240530-171200-171500
REC000T-20240530-171500-171800
REC000T-20240530-171800-172100
REC000T-20240530-172100-172400
REC000T-20240530-172400-172700
REC000T-20240530-172700-173000
REC000T-20240530-173000-173300
REC000T-20240530-173300-173600
REC000T-20240530-173600-173900
REC000T-20240530-173900-174200
REC000T-20240530-174200-174500
REC000T-20240530-174500-174800
REC000T-20240530-174800-175100
REC000T-20240530-175100-175400
REC000T-20240530-175400-175700
REC000T-20240530-175700-180000
REC000T-20240530-180000-180300
REC000T-20240530-180300-180600
REC000T-20240530-180600-180900
REC000T-20240530-180900-181200
REC000T-20240530-181200-181500
REC000T-20240530-181500-181800
REC000T-20240530-181800-182100
REC000T-20240530-182100-182400
REC000T-20240530-182400-182700
REC000T-20240530-182700-183000
REC000T-20240530-183000-183300
REC000T-20240530-183300-183600
REC000T-20240530-183600-183900
REC000T-20240530-183900-183900
"

for filename in $INPUT_FILE
do
     echo "$filename"
    ./VideoCalc -i=$INPUT_PATH/$filename.avi -o=$INPUT_PATH/$filename.av1 \
                -w=2560 -h=1440 -m=6 -f=500000 --fps=15
    #ffmpeg -hide_banner -i $INPUT_PATH/$filename.av1 -f null -
    ffmpeg -hide_banner -r 15 -i $INPUT_PATH/$filename.av1 -vcodec copy -y $INPUT_PATH/$filename.mp4
    rm $INPUT_PATH/$filename.av1
    rm $INPUT_PATH/$filename.avi
done