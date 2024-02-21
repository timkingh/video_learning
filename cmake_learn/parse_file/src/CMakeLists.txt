
include_directories(../lib)
#link_directories(../lib)

set(SRC_LIST main.c)

add_executable(test ${SRC_LIST})

#add_library(test_lib SHARED ${SRC_LIST})

target_link_libraries(test hello)
#target_link_libraries(test_lib hello)