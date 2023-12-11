CC = g++
OBJ = calc_hist.o calc_psnr.o calc_var.o quant_mtx.o common.o weight_yuv.o \
      video_calc.o cdef_dir.o

VideoCalc : $(OBJ)
	${CC} -std=c++11 -o VideoCalc $(OBJ)

video_calc.o : VideoCalc.cpp
	${CC} -std=c++11 -c VideoCalc.cpp -g -o video_calc.o

calc_hist.o : calc_histogram.cpp calc_histogram.h
	${CC} -std=c++11 -c calc_histogram.cpp -g -o calc_hist.o

calc_psnr.o : calc_psnr.cpp getopt.hpp
	${CC} -std=c++11 -c calc_psnr.cpp -g -o calc_psnr.o

calc_var.o : calc_var.cpp calc_var.h
	${CC} -std=c++11 -c calc_var.cpp -g -o calc_var.o

cdef_dir.o : cdef_dir.cpp common.o
	${CC} -std=c++11 -c cdef_dir.cpp -g -o cdef_dir.o

quant_mtx.o : quant_matrix.cpp quant_matrix.h
	${CC} -std=c++11 -c quant_matrix.cpp -g -o quant_mtx.o

common.o : video_common.cpp video_common.h
	${CC} -std=c++11 -c video_common.cpp -g -o common.o

weight_yuv.o : weight_yuv.cpp weight_yuv.h
	${CC} -std=c++11 -c weight_yuv.cpp -g -o weight_yuv.o

clean :
	rm -rf VideoCalc *.o
