CC = g++
CFLAGS = -Wall -g -std=c++11
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

VideoCalc: $(SOURCES:.cpp=.o)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.exe
