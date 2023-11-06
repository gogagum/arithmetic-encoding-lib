#ifndef ENCODE_DECODE_TEST_HPP
#define ENCODE_DECODE_TEST_HPP

#include <gtest/gtest.h>

#include <ord_generator.hpp>
#include <random>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
class EncodeDecodeTest : public testing::Test {
 public:
  const static std::vector<std::uint64_t> smallSequence;
  static std::mt19937 gen;  // NOLINT
  static auto generateEncoded(std::size_t length, std::uint32_t rng);

  std::vector<std::uint64_t> decoded;
  std::back_insert_iterator<std::vector<std::uint64_t>> outIter{decoded};
};

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
// NOLINTBEGIN
template <class DictT>
const std::vector<std::uint64_t> EncodeDecodeTest<DictT>::smallSequence{ 
    5, 3, 5, 7, 2};
// NOLINTEND

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
std::mt19937 EncodeDecodeTest<DictT>::gen{42};  // NOLINT

#endif  // ENCODE_DECODE_TEST_HPP
