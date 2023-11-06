#ifndef ENCODE_DECODE_TEST_HPP
#define ENCODE_DECODE_TEST_HPP

#include <gtest/gtest.h>

#include <ord_generator.hpp>
#include <random>
#include <ranges>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
class EncodeDecodeTest : public testing::Test {
 public:
  const static std::vector<std::uint64_t> smallSequence;
  static std::mt19937 gen;  // NOLINT

  static auto generateEncoded(std::size_t length, std::uint32_t rng);

  // NOLINTBEGIN
  template <std::uint64_t minAlphabetSize = 10,
            std::uint64_t maxAlphabetSize = 256, std::size_t minLength = 0,
            std::uint64_t maxLength = 500>
  void refreshForFuzzTest();
  // NOLINTEND

  std::uint64_t maxOrd;
  std::vector<std::uint64_t> encoded{};
  std::vector<std::uint64_t> decoded{};
  std::back_insert_iterator<std::vector<std::uint64_t>> outIter{decoded};
};

////////////////////////////////////////////////////////////////////////////////
// NOLINTBEGIN
template <class DictT>
const std::vector<std::uint64_t> EncodeDecodeTest<DictT>::smallSequence{5, 3, 5,
                                                                        7, 2};
// NOLINTEND

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
std::mt19937 EncodeDecodeTest<DictT>::gen{42};  // NOLINT

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
auto EncodeDecodeTest<DictT>::generateEncoded(std::size_t length,
                                              std::uint32_t rng) {
  auto encoded = std::vector<std::uint64_t>{};

  std::generate_n(std::back_inserter(encoded), length,
                  ael::test::OrdGenerator(gen, rng));
  return encoded;
}

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
template <std::uint64_t minAlphabetSize, std::uint64_t maxAlphabetSize,
          std::size_t minLength, std::uint64_t maxLength>
void EncodeDecodeTest<DictT>::refreshForFuzzTest() {
  static_assert(minAlphabetSize <= maxAlphabetSize, "Invalid alphabet sizes.");
  static_assert(minLength <= maxLength, "Invalid lengths.");

  decoded.clear();
  maxOrd = std::uniform_int_distribution<std::uint64_t>(minAlphabetSize,
                                                        maxAlphabetSize)(gen);

  const auto length =
      std::uniform_int_distribution<std::size_t>(minLength, maxLength)(gen);

  encoded = generateEncoded(length, maxOrd);
}

#endif  // ENCODE_DECODE_TEST_HPP
