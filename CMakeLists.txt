cmake_minimum_required(VERSION 3.15)

project(arithmetic-encoding-lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_library(${PROJECT_NAME} STATIC)

add_subdirectory(thirdparty)

target_sources(${PROJECT_NAME}
    PRIVATE
        src/arithmetic_coder.cpp
        src/arithmetic_decoder.cpp
        src/a_d_dictionary_base.cpp
        src/adaptive_a_dictionary.cpp
        src/adaptive_d_dictionary.cpp
        src/adaptive_dictionary.cpp
        src/byte_data_constructor.cpp
        src/cumulative_count.cpp
        src/cumulative_unique_count.cpp
        src/data_parser.cpp
        src/decreasing_on_update_dictionary.cpp
        src/multiply_and_divide.cpp
        src/ppma_dictionary.cpp
        src/ppmd_dictionary.cpp
        src/ppm_a_d_dictionary_base.cpp
        src/static_dictionary.cpp
        src/uniform_dictionary.cpp
        src/adaptive_dictionary_base.cpp
        src/ranges_calc.cpp
        src/esc_adaptive_a_dictionary.cpp
        src/esc_adaptive_d_dictionary.cpp
        src/esc_a_d_dictionary_base.cpp
        src/esc_ppma_dictionary.cpp
        src/esc_ppmd_dictionary.cpp
        src/esc_ppm_a_d_dictionary_base.cpp
        src/ctx_base.cpp
        src/max_ord_base.cpp
        src/contextual_dictionary_stats_base.cpp
        src/contextual_dictionary_base_improved.cpp
        src/contextual_dictionary_base.cpp
        src/word_probability_stats.cpp
        src/numerical_coder.cpp
        src/numerical_decoder.cpp
        src/esc_arithmetic_coder.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# without it public headers won't get installed
set(public_headers
    include/ael/arithmetic_coder.hpp
    include/ael/arithmetic_decoder.hpp
    include/ael/byte_data_constructor.hpp
    include/ael/data_parser.hpp
    include/ael/numerical_coder.hpp
    include/ael/numerical_decoder.hpp
    include/ael/dictionary/uniform_dictionary.hpp
    include/ael/dictionary/static_dictionary.hpp
    include/ael/dictionary/ppma_dictionary.hpp
    include/ael/dictionary/ppmd_dictionary.hpp
    include/ael/dictionary/adaptive_dictionary.hpp
    include/ael/dictionary/adaptive_a_contextual_dictionary.hpp
    include/ael/dictionary/adaptive_a_contextual_dictionary_improved.hpp
    include/ael/dictionary/adaptive_a_dictionary.hpp
    include/ael/dictionary/adaptive_d_contextual_dictionary.hpp
    include/ael/dictionary/adaptive_d_contextual_dictionary_improved.hpp
    include/ael/dictionary/adaptive_d_dictionary.hpp
    include/ael/dictionary/decreasing_counts_dictionary.hpp
    include/ael/dictionary/decreasing_on_update_dictionary.hpp
    include/ael/esc/arithmetic_coder.hpp
    include/ael/esc/arithmetic_decoder.hpp
    include/ael/esc/dictionary/adaptive_a_dictionary.hpp
    include/ael/esc/dictionary/adaptive_d_dictionary.hpp
    include/ael/esc/dictionary/ppma_dictionary.hpp
    include/ael/esc/dictionary/ppmd_dictionary.hpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

find_library(dst dynamic-segment-tree)

target_link_libraries(arithmetic-encoding-lib
    Boost::config
    Boost::multiprecision
    Boost::container
    Boost::container_hash
    dynamic-segment-tree)

if (AEL_TESTS)
    add_subdirectory(test)
endif ()

if (AEL_BENCHMARK)
    add_subdirectory(benchmark)
endif()
