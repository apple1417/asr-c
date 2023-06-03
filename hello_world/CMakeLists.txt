cmake_minimum_required(VERSION 3.23)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.c *.cpp)
add_executable(asr_hello_world ${sources})
