yuv_process : yuv_process.o rectangle.o
	g++ -std=c++11 -o yuv_process yuv_process.o rectangle.o

yuv_process.o : yuv_process.cpp getopt.hpp rectangle.h
	gcc -std=c++11 -c yuv_process.cpp	

rectangle.o : rectangle.cpp rectangle.h
	gcc -std=c++11 -c rectangle.cpp
clean :
	rm -f *.o yuv_process
