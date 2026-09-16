find_package(GTest REQUIRED)
find_package(fmt REQUIRED)

enable_testing()

add_executable(ael-tests
    byte_data_constructor.cpp
    data_parser.cpp
    context_buffer.cpp
    no_esc/adaptive_dictionary.cpp
    no_esc/static_dictionary.cpp
    no_esc/uniform_dictionary.cpp
    no_esc/adaptive_dictionary.cpp
    no_esc/adaptive_a_dictionary.cpp
    no_esc/adaptive_d_dictionary.cpp
    no_esc/ppma_dictionary.cpp
    no_esc/ppmd_dictionary.cpp
    no_esc/adaptive_a_contextual_dictionary.cpp
    no_esc/adaptive_a_contextual_dictionary_improved.cpp
    no_esc/adaptive_d_contextual_dictionary.cpp
    no_esc/adaptive_d_contextual_dictionary_improved.cpp
    no_esc/decreasing_counts_dictionary.cpp
    no_esc/decreasing_on_update_dictionary.cpp
    esc/adaptive_a_dictionary.cpp
    esc/adaptive_d_dictionary.cpp
    esc/ppma_dictionary.cpp
    esc/ppmd_dictionary.cpp
    numerical_coder.cpp
    numerical_decoder.cpp
    encode_decode/no_esc/adaptive_a_d.cpp
    encode_decode/no_esc/adaptive.cpp
    encode_decode/no_esc/ppm_a_d.cpp
    encode_decode/no_esc/adaptive_a_d_contextual.cpp
    encode_decode/no_esc/uniform.cpp
    encode_decode/no_esc/static.cpp
    encode_decode/numerical.cpp
    encode_decode/esc/ppm_a_d.cpp
    encode_decode/esc/adaptive_a_d.cpp
)

target_include_directories(ael-tests PRIVATE lib)
target_link_libraries(ael-tests LINK_PUBLIC gtest_main fmt::fmt arithmetic-encoding-lib)

include(GoogleTest)
gtest_discover_tests(ael-tests)
