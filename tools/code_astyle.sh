#!/bin/bash
set -e
./astyle --quiet --options=astyle_x264 ../*.cpp ../*.c ../*.h

set -e
./astyle --quiet --options=astyle_jm ../*.cpp ../*.c ../*.h
