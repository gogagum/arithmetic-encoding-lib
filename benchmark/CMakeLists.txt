cmake_minimum_required(VERSION 3.15)

include(FetchContent)
FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.7.1.zip
)

FetchContent_MakeAvailable(googlebenchmark)

add_library(benchmark_lib STATIC lib/include/two_parts_source.hpp)

set_target_properties(benchmark_lib PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(test)

add_executable(benchmark_adaptive_coder adaptive_coder.cpp)
target_link_libraries(benchmark_adaptive_coder LINK_PUBLIC benchmark::benchmark benchmark_lib)

