CC=g++
OBJS=generate_yuv.o

md_test:$(OBJS)
	${CC} -o generate_yuv $(OBJS)

md_test.o: generate_yuv.cpp generate_yuv.h getopt.hpp
	${CC} -c generate_yuv.cpp -std=c++11 -g

clean:
	rm generate_yuv generate_yuv.o

