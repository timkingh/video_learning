# compiler and linker flags
CC = gcc
CXX = g++
CFLAGS = -Wall -g -I ./ffmpeg/include \
         -L ./ffmpeg/lib -lavformat -lavcodec \
         -lavfilter -lavutil -lswscale -lswresample \
		 -Wl,-rpath,./ffmpeg/lib
CXXFLAGS = -Wall -g -std=c++11 -I ./ffmpeg/include \
         -L ./ffmpeg/lib -lavformat -lavcodec \
         -lavfilter -lavutil -lswscale -lswresample \
		 -Wl,-rpath,./ffmpeg/lib

# source files
C_SOURCES = $(wildcard *.c)
CXX_SOURCES = $(wildcard *.cpp)

# object files
C_OBJECTS = $(C_SOURCES:.c=.o)
CXX_OBJECTS = $(CXX_SOURCES:.cpp=.o)

# executable file
EXECUTABLE = ff_tools

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE):$(C_OBJECTS) $(CXX_OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

# compile C source files
%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

# compile C++ source files
%.o: %.cpp
	$(CXX) -o $@ $(CXXFLAGS) -c $<

clean:
	rm -f $(EXECUTABLE)$(C_OBJECTS) $(CXX_OBJECTS)
