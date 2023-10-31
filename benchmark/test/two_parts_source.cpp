#include <gtest/gtest.h>

#include <ranges>
#include <two_parts_source.hpp>

// NOLINTBEGIN(cppcoreguidelines-owning-memory,
// cppcoreguidelines-avoid-magic-numbers)

TEST(TwoPartsSource, Construct) {
  auto src = TwoPartsSource::getGeneration({64, 6, 5.7, 34});
}

TEST(TwoPartsSource, InvalidEntropy) {
  EXPECT_THROW(auto src = TwoPartsSource::getGeneration({64, 6, 6.1, 42}),
               std::logic_error);
}

TEST(TwoPartsSource, ConstructGenerationInstance) {
  auto src = TwoPartsSource::getGeneration({64, 8, 5.5, 75});
}

TEST(TwoPartsSource, IterateGenerationInstance) {
  auto src = TwoPartsSource::getGeneration({64, 8, 5.5, 75});
  for (auto ord : src) {
  }
}

TEST(TwoPartsSource, IterateGenerationInstanceLength) {
  auto src = TwoPartsSource::getGeneration({64, 8, 5.5, 5, 75});
  std::vector<std::uint32_t> generated;
  std::ranges::copy(src.begin(), src.end(), std::back_inserter(generated));
  EXPECT_EQ(generated.size(), 5);
}

TEST(TwoPartsSource, IterateGenerationInstanceLengthRangesLibCopy) {
  auto src = TwoPartsSource::getGeneration({64, 8, 5.5, 5, 75});
  std::vector<std::uint32_t> generated;
  std::ranges::copy(src.begin(), src.end(), std::back_inserter(generated));
  EXPECT_EQ(generated.size(), 5);
}

TEST(TwoPartsSource, IterateGenerationInstanceLengthRanges) {
  auto src = TwoPartsSource::getGeneration({64, 8, 5.5, 5, 75});
  std::vector<std::uint32_t> generated;
  std::ranges::copy(std::move(src), std::back_inserter(generated));
  EXPECT_EQ(generated.size(), 5);
}

TEST(TwoPartsSource, ResultingEntropy) {
  std::map<std::uint64_t, std::size_t> counts;
  std::size_t totalCount = 0;

  auto src = TwoPartsSource::getGeneration({256, 16, 7.1, 100000, 23});
  for (auto ord: src) {
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

// NOLINTEND(cppcoreguidelines-owning-memory,
// cppcoreguidelines-avoid-magic-numbers)
