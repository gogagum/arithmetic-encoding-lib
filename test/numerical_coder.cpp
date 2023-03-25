#include "ael/byte_data_constructor.hpp"
#include <cstdint>
#include <gtest/gtest.h>

#include <ael/numerical_coder.hpp>

////////////////////////////////////////////////////////////////////////////////
TEST(NumericalCoder, EmptyEncode) {
    auto sequence = std::vector<std::uint32_t>();
    auto countsMapping = std::vector<ael::NumericalCoder::CountEntry>();
    auto dataConstructor = ael::ByteDataConstructor();
    ael::NumericalCoder::encode(sequence, countsMapping, dataConstructor);
}

////////////////////////////////////////////////////////////////////////////////
TEST(NumericalCoder, EncodeSmallSequence) {
    auto sequence = std::vector<std::uint32_t>{5, 6, 2, 5, 3, 2};
    auto countsMapping = ael::NumericalCoder::countWords(sequence);
    auto dataConstructor = ael::ByteDataConstructor();
    ael::NumericalCoder::encode(sequence, countsMapping, dataConstructor);
}
