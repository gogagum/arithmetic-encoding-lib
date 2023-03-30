#include <gtest/gtest.h>

#include <ael/numerical_coder.hpp>
#include <cstdint>

#include "ael/byte_data_constructor.hpp"

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

TEST(NumericalCoder, EmptyEncode) {
  auto sequence = std::vector<std::uint32_t>();
  auto countsMapping = std::vector<ael::NumericalCoder::CountEntry>();
  auto dataConstructor = ael::ByteDataConstructor();
  ael::NumericalCoder::encode(sequence, countsMapping, dataConstructor);
}

TEST(NumericalCoder, EncodeSmallSequence) {
  auto sequence = std::vector<std::uint32_t>{5, 6, 2, 5, 3, 2};
  auto countsMapping = ael::NumericalCoder::countWords(sequence);
  auto dataConstructor = ael::ByteDataConstructor();
  ael::NumericalCoder::encode(sequence, countsMapping, dataConstructor);
}

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
