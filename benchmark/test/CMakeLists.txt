include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)

FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    fmt
    URL https://github.com/fmtlib/fmt/releases/download/9.1.0/fmt-9.1.0.zip
)

FetchContent_MakeAvailable(fmt)

enable_testing()
add_executable(benchmark-lib-tests two_parts_source.cpp)
target_include_directories(benchmark-lib-tests PRIVATE "../lib/include")
target_link_libraries(benchmark-lib-tests LINK_PUBLIC gtest_main benchmark_lib fmt::fmt)

include(GoogleTest)
gtest_discover_tests(benchmark-lib-tests)
