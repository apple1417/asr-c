cmake_minimum_required(VERSION 3.23)

add_link_options(-mexec-model=reactor -Wl,--no-entry,--allow-undefined-file=${CMAKE_CURRENT_SOURCE_DIR}/asr/asr.syms)

# Project specific options
project(asr-c-template)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

include_directories(asr)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/*.c" "src/*.cpp")
add_executable(asr-c-template ${sources})
