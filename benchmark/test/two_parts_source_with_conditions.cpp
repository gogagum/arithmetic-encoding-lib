#include <gtest/gtest.h>

#include <array>
#include <boost/range/combine.hpp>
#include <ranges>
#include <two_parts_source_with_conditions.hpp>

// NOLINTBEGIN(cppcoreguidelines-owning-memory,
// cppcoreguidelines-avoid-magic-numbers)

TEST(TwoPartsSourceWithConditions, Construct) {
  auto cfg =
      TwoPartsSourceWithConditions::GenerationConfig{64, 6, 5.7, 5.1, 42, 57};
  auto src = TwoPartsSourceWithConditions::getGeneration(cfg);
}

TEST(TwoPartsSourceWithConditions, InvalidEntropy) {
  auto cfg =
      TwoPartsSourceWithConditions::GenerationConfig{64, 6, 6.1, 5.1, 42, 57};
  EXPECT_THROW(auto src = TwoPartsSourceWithConditions::getGeneration(cfg),
               std::logic_error);
}

TEST(TwoPartsSourceWithConditions, InvalidConditionalEntropy) {
  auto cfg =
      TwoPartsSourceWithConditions::GenerationConfig{64, 6, 5.7, 5.8, 42, 57};
  EXPECT_THROW(auto src = TwoPartsSourceWithConditions::getGeneration(cfg),
               std::logic_error);
}

TEST(TwoPartsSourceWithConditions, ConstructGenerationInstance) {
  auto src =
      TwoPartsSourceWithConditions::getGeneration({64, 6, 5.7, 5.1, 42, 57});
}

TEST(TwoPartsSourceWithConditions, IterateGenerationInstance) {
  auto src = TwoPartsSourceWithConditions::getGeneration({64, 8, 5.5, 5.4, 75});
  for (auto ord : src) {
  }
}

TEST(TwoPartsSourceWithConditions, IterateGenerationInstanceLength) {
  auto src =
      TwoPartsSourceWithConditions::getGeneration({64, 8, 5.5, 5.4, 5, 75});
  std::vector<std::uint32_t> generated;
  std::ranges::copy(src.begin(), src.end(), std::back_inserter(generated));
  EXPECT_EQ(generated.size(), 5);
}

TEST(TwoPartsSourceWithConditions,
     IterateGenerationInstanceLengthRangesLibCopy) {
  auto src =
      TwoPartsSourceWithConditions::getGeneration({64, 8, 5.5, 5.4, 5, 75});
  std::vector<std::uint32_t> generated;
  std::ranges::copy(src.begin(), src.end(), std::back_inserter(generated));
  EXPECT_EQ(generated.size(), 5);
}

TEST(TwoPartsSourceWithConditions, IterateGenerationInstanceLengthRanges) {
  auto src =
      TwoPartsSourceWithConditions::getGeneration({64, 8, 5.5, 5.4, 5, 75});
  std::vector<std::uint32_t> generated;
  std::ranges::copy(std::move(src), std::back_inserter(generated));
  EXPECT_EQ(generated.size(), 5);
}

TEST(TwoPartsSourceWithConditions, ResultingEntropy) {
  std::map<std::uint64_t, std::size_t> counts;
  std::size_t totalCount = 0;

  auto src = TwoPartsSourceWithConditions::getGeneration(
      {256, 16, 7.1, 6.9, 100000, 23});
  for (auto ord : src) {
    ++totalCount;
    ++counts[ord];
  }

  const auto pLogPs =
      counts | std::views::transform([totalCount](const auto& entry) {
        auto [ord, count] = entry;
        const double countDouble = static_cast<double>(count);
        const double p = countDouble / totalCount;
        return -p * std::log2(p);
      });

  double entropyEstimation = std::accumulate(pLogPs.begin(), pLogPs.end(), 0.);

  EXPECT_TRUE((entropyEstimation - 7.1) / 7.1 < 0.1);
}

template <class I, class S, std::ranges::subrange_kind K>
struct boost::range_iterator<std::ranges::subrange<I, S, K>> {
  using type = I;
};

TEST(TwoPartsSourceWithConditions, ResultingConditionalEntropy) {
  std::size_t totalCount = 0;

  auto src = TwoPartsSourceWithConditions::getGeneration(
      {256, 16, 7.1, 6.9, 100000, 23});

  auto values = std::array<std::uint64_t, 100000>();
  std::ranges::copy(src, values.begin());

  auto firsts = std::ranges::subrange(values.begin(), values.end() - 1);
  auto seconds = std::ranges::subrange(values.begin() + 1, values.end());

  std::map<std::pair<std::uint64_t, std::uint64_t>, std::size_t> counts;

  for (auto&& [v0, v1] : boost::range::combine(firsts, seconds)) {
    ++totalCount;
    ++counts[std::make_pair(v0, v1)];
  }

  const auto pLogPs =
      counts | std::views::transform([totalCount](const auto& entry) {
        auto [ord, count] = entry;
        const double countDouble = static_cast<double>(count);
        const double p = countDouble / totalCount;
        return -p * std::log2(p);
      });

  double entropyEstimation =
      std::accumulate(pLogPs.begin(), pLogPs.end(), 0.) - 7.1;

  EXPECT_TRUE((entropyEstimation - 6.9) / 6.9 < 0.1);
}

// NOLINTEND(cppcoreguidelines-owning-memory,
// cppcoreguidelines-avoid-magic-numbers)
