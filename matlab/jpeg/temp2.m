clear all
clc

min_v = 0;
max_v = 255;

blk8 = [
   min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
   max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
   min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
   max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
   min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
   max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
   min_v   max_v   min_v   max_v   min_v   max_v   min_v   max_v
   max_v   min_v   max_v   min_v   max_v   min_v   max_v   min_v
];

T = dctmtx(8);

coef = T * (blk8 - 128) * T'