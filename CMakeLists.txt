cmake_minimum_required(VERSION 3.23)

project(asr-c-template)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(asr)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/*.c" "src/*.cpp")
add_executable(asr-c-template ${sources})
target_link_libraries(asr-c-template asr)
