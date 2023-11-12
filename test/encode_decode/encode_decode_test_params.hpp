#ifndef AEL_TEST_ENCODE_DECODE_TEST_PARAM_HPP
#define AEL_TEST_ENCODE_DECODE_TEST_PARAM_HPP

#include <fmt/format.h>

#include <algorithm>
#include <cstdint>
#include <random>
#include <ranges>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
/// \brief The EncodeDecodeTestParam structure
struct EncodeDecodeTestParams {
  std::string description{};
  std::vector<std::uint64_t> encoded{};
};

class GenerateEncodeDecodeTests {
 public:
  explicit GenerateEncodeDecodeTests(std::uint64_t maxOrd,
                                     std::uint64_t seed = 0);

  std::vector<EncodeDecodeTestParams> ofSizes(std::vector<std::size_t> sizes);

 private:
  std::uint64_t maxOrd_;
  std::mt19937 gen_;
};

////////////////////////////////////////////////////////////////////////////////
GenerateEncodeDecodeTests::GenerateEncodeDecodeTests(std::uint64_t maxOrd,
                                                     std::uint64_t seed)
    : maxOrd_(maxOrd), gen_(seed) {
}

////////////////////////////////////////////////////////////////////////////////
std::vector<EncodeDecodeTestParams> GenerateEncodeDecodeTests::ofSizes(
    std::vector<std::size_t> sizes) {
  const auto generateOne = [this](std::size_t size) -> EncodeDecodeTestParams {
    auto encoded = std::vector<std::uint64_t>{};
    for (auto _ : std::ranges::iota_view(std::size_t{0}, size)) {
      encoded.push_back(gen_() % maxOrd_);
    }
    return {
        fmt::format("RandomEncodeDecodeOfSize{}WithMaxOrd{}", size, maxOrd_),
        encoded};
  };
  auto ret = std::vector<EncodeDecodeTestParams>{};
  std::ranges::transform(sizes, std::back_inserter(ret), generateOne);
  return ret;
}

#endif  // AEL_TEST_ENCODE_DECODE_TEST_PARAM_HPP
