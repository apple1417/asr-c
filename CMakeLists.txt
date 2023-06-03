cmake_minimum_required(VERSION 3.23)

project(asr_c)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(asr)

add_subdirectory(asr_asl)
target_link_libraries(asr_asl PRIVATE asr)

add_subdirectory(asr_utils)
target_link_libraries(asr_utils PRIVATE asr)

add_subdirectory(hello_world)
target_link_libraries(asr_hello_world PRIVATE asr)
