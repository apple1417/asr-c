cmake_minimum_required(VERSION 3.23)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.h *.cpp)
add_library(asr_utils OBJECT ${sources})
set_target_properties(asr_utils PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION True
)

target_compile_features(asr_utils PUBLIC cxx_std_20)

# Enable "experiemental" features for std::format
# Upstream libc++ fully supports it, just still off by default in the wasi sysroot
target_compile_definitions(asr_utils PUBLIC _LIBCPP_ENABLE_EXPERIMENTAL)

target_include_directories(asr_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_precompile_headers(asr_utils PUBLIC utils/pch.h)
