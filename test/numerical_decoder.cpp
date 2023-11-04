#include <gtest/gtest.h>

#include <ael/numerical_decoder.hpp>
#include <array>
#include <cstdint>
#include <iterator>

#include "ael/data_parser.hpp"

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

TEST(NumericalDecoder, DecodeEmptySequence) {
  auto data = std::array<std::byte, 0>();
  auto dataParser = ael::DataParser(data);
  auto ret = std::vector<std::uint32_t>();
  ael::NumericalDecoder(dataParser, 0, 0, 0).decode(std::back_inserter(ret), 5);
}

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
