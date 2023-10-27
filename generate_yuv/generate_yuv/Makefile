CC=g++
OBJS=generate_yuv.o

generate_yuv:$(OBJS)
	${CC} -o generate_yuv $(OBJS)

generate_yuv.o: generate_yuv.cpp generate_yuv.h getopt.hpp
	${CC} -c generate_yuv.cpp -std=c++11 -g

clean:
	rm generate_yuv generate_yuv.o

