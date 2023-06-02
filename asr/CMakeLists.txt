cmake_minimum_required(VERSION 3.23)

add_library(asr INTERFACE asr.h)
target_include_directories(asr INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_options(asr INTERFACE
    -mexec-model=reactor
    -Wl,--no-entry
    -Wl,--allow-undefined-file=${CMAKE_CURRENT_SOURCE_DIR}/asr.syms
)
