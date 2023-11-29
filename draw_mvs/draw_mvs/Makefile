CC = g++
OBJ = common.o draw_mvs.o rectangle.o

draw_mvs : $(OBJ)
	${CC} -std=c++11 -o draw_mvs $(OBJ)

draw_mvs.o : draw_mvs.cpp
	${CC} -std=c++11 -c draw_mvs.cpp -g -o draw_mvs.o

rectangle.o : rectangle.cpp rectangle.h
	${CC} -std=c++11 -c rectangle.cpp -g -o rectangle.o

common.o : common.cpp common.h
	${CC} -std=c++11 -c common.cpp -g -o common.o

clean :
	rm -rf draw_mvs *.o
