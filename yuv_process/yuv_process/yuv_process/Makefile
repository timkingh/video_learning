objects = yuv_process.o rectangle.o common.o

yuv_process : $(objects)
	g++ -std=c++11 -o yuv_process $(objects)

yuv_process.o : yuv_process.cpp getopt.hpp 
	g++ -std=c++11 -c yuv_process.cpp	

rectangle.o : rectangle.cpp rectangle.h
	g++ -std=c++11 -c rectangle.cpp

common.o : common.cpp common.h
	g++ -std=c++11 -c common.cpp
clean :
	rm -f *.o yuv_process
