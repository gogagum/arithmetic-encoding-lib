include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(benchmark-lib-tests two_parts_source.cpp two_parts_source_with_conditions.cpp)
target_link_libraries(benchmark-lib-tests LINK_PUBLIC gtest_main benchmark_lib)

include(GoogleTest)
gtest_discover_tests(benchmark-lib-tests)
