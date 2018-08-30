#!/bin/bash
set -e
./astyle --recursive --options=tim_astyle ../../*.cpp ../../*.c ../../*.h ../../*.hpp 

#set -e
#./astyle --quiet --options=astyle_jm ../*.cpp ../*.c ../*.h
