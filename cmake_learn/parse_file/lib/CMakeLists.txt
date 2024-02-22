
#add_definitions(-fPIC)
#add_definitions(-shared)

add_library(hello SHARED hello.c)
add_library(hello_static STATIC hello.c)
#set_target_properties(hello PROPERTIES VERSION 0.12)
set_target_properties(hello_static PROPERTIES OUTPUT_NAME hello)