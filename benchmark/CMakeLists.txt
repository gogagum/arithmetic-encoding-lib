cmake_minimum_required(VERSION 3.15)

include(FetchContent)
FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.8.2.zip
)

FetchContent_MakeAvailable(googlebenchmark)

FetchContent_Declare(
    fmt
    URL https://github.com/fmtlib/fmt/releases/download/10.0.0/fmt-10.0.0.zip
)

FetchContent_MakeAvailable(fmt)

add_library(benchmark_lib STATIC)

set(benchmark_lib_public_headers lib/include/two_parts_source.hpp)

target_sources(benchmark_lib
    PRIVATE
        lib/src/two_parts_source.cpp
        lib/src/two_parts_source_with_conditions.cpp
        lib/src/source_entropy_base.cpp
)

set_target_properties(benchmark_lib PROPERTIES PUBLIC_HEADER "${benchmark_lib_public_headers}")

target_include_directories(benchmark_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)


set_target_properties(benchmark_lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(benchmark_lib LINK_PUBLIC fmt::fmt)

add_subdirectory(test)

add_executable(benchmark_adaptive_coders adaptive_coders.cpp)
add_executable(benchmark_adaptive_decoders adaptive_decoders.cpp)

target_link_libraries(benchmark_adaptive_coders LINK_PUBLIC benchmark::benchmark benchmark_lib arithmetic-encoding-lib)
target_link_libraries(benchmark_adaptive_decoders LINK_PUBLIC benchmark::benchmark benchmark_lib arithmetic-encoding-lib)
