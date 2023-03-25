#include "ael/data_parser.hpp"
#include <array>
#include <cstdint>
#include <gtest/gtest.h>

#include <ael/numerical_decoder.hpp>
#include <iterator>

////////////////////////////////////////////////////////////////////////////////
TEST(NumericalDecoder, DecodeEmptySequence) {
    auto data = std::array<std::byte, 0>();
    auto dataParser = ael::DataParser(data);
    auto ret = std::vector<std::uint32_t>();
    ael::NumericalDecoder::decode(dataParser, std::back_inserter(ret), 5, ael::NumericalDecoder::LayoutInfo{0, 0, 0, 0, 0});
}