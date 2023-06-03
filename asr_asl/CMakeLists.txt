cmake_minimum_required(VERSION 3.23)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../asr ${CMAKE_CURRENT_BINARY_DIR}/asr)

add_library(asr_asl OBJECT asl.c asl.h)
target_include_directories(asr_asl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(asr_asl PUBLIC asr)
