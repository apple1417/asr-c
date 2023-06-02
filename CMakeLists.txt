cmake_minimum_required(VERSION 3.23)

project(asr_hello_world)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(asr)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "hello_world/*.c" "hello_world/*.cpp")
add_executable(asr_hello_world ${sources})
target_link_libraries(asr_hello_world asr)
