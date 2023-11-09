#include <gtest/gtest.h>

#include <ael/numerical_coder.hpp>
#include <cstdint>
#include <set>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

TEST(NumericalCoder, EmptyEncode) {
  auto sequence = std::vector<std::uint32_t>();
  auto countsMapping = std::vector<
      ael::NumericalCoder::CountEntry>();
  ael::NumericalCoder().encode(sequence, countsMapping);
}

TEST(NumericalCoder, EncodeSmallSequence) {
  auto sequence = std::vector<std::uint32_t>{5, 6, 2, 5, 3, 2};
  auto countsMapping = ael::NumericalCoder::countWords(sequence);
  ael::NumericalCoder().encode(sequence, countsMapping);
}

TEST(NumericalCoder, EncodeSmallSequenceWithTicks) {
  auto sequence = std::vector<std::uint32_t>{5, 6, 2, 5, 3, 2};
  auto countsMapping = ael::NumericalCoder::countWords(sequence);

  auto n = std::size_t{0};

  auto tick = [&n]() {
    ++n;
  };

  ael::NumericalCoder().encode(sequence, countsMapping, tick, []{}, []{});
  EXPECT_EQ(n, std::set(sequence.begin(), sequence.end()).size());
}

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
