calc_psnr : calc_psnr.o
	 g++ -std=c++11 -o calc_psnr calc_psnr.o
calc_psnr.o : calc_psnr.cpp getopt.hpp
	 g++ -std=c++11 -c calc_psnr.cpp
clean :
	rm -rf calc_psnr *.o
