#include <gtest/gtest.h>

#include <array>
#include <ranges>
#include <two_parts_source_with_conditions.hpp>

// NOLINTBEGIN(cppcoreguidelines-owning-memory,
// cppcoreguidelines-avoid-magic-numbers)

TEST(TwoPartsSourceWithConditions, Construct) {
  auto src =
      TwoPartsSourceWithConditions::getGeneration(64, 6, 5.7, 5.1, 42, 57);
}

TEST(TwoPartsSourceWithConditions, InvalidEntropy) {
  EXPECT_THROW(auto src = TwoPartsSourceWithConditions::getGeneration(
                   64, 6, 6.1, 5.1, 42, 57),
               std::logic_error);
}

TEST(TwoPartsSourceWithConditions, InvalidConditionalEntropy) {
  EXPECT_THROW(auto src = TwoPartsSourceWithConditions::getGeneration(
                   64, 6, 5.7, 5.8, 42, 57),
               std::logic_error);
}

TEST(TwoPartsSourceWithConditions, ConstructGenerationInstance) {
  auto src =
      TwoPartsSourceWithConditions::getGeneration(64, 6, 5.7, 5.1, 42, 57);
}

TEST(TwoPartsSourceWithConditions, IterateGenerationInstance) {
  auto src = TwoPartsSourceWithConditions::getGeneration(64, 8, 5.5, 5.4, 75);
  for (auto ord : src) {
  }
}

TEST(TwoPartsSourceWithConditions, IterateGenerationInstanceLength) {
  auto src =
      TwoPartsSourceWithConditions::getGeneration(64, 8, 5.5, 5.4, 5, 75);
  std::vector<std::uint32_t> generated;
  std::ranges::copy(src.begin(), src.end(), std::back_inserter(generated));
  EXPECT_EQ(generated.size(), 5);
}

TEST(TwoPartsSourceWithConditions,
     IterateGenerationInstanceLengthRangesLibCopy) {
  auto src =
      TwoPartsSourceWithConditions::getGeneration(64, 8, 5.5, 5.4, 5, 75);
  std::vector<std::uint32_t> generated;
  std::ranges::copy(src.begin(), src.end(), std::back_inserter(generated));
  EXPECT_EQ(generated.size(), 5);
}

TEST(TwoPartsSourceWithConditions, IterateGenerationInstanceLengthRanges) {
  auto src =
      TwoPartsSourceWithConditions::getGeneration(64, 8, 5.5, 5.4, 5, 75);
  std::vector<std::uint32_t> generated;
  std::ranges::copy(std::move(src), std::back_inserter(generated));
  EXPECT_EQ(generated.size(), 5);
}

TEST(TwoPartsSourceWithConditions, ResultingEntropy) {
  std::map<std::uint64_t, std::size_t> counts;
  std::size_t totalCount = 0;

  auto src = TwoPartsSourceWithConditions::getGeneration(256, 16, 7.1, 6.9,
                                                         100000, 23);
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

TEST(TwoPartsSourceWithConditions, ResultingConditionalEntropy) {
  constexpr std::size_t totalCount = 100000;

  auto src = TwoPartsSourceWithConditions::getGeneration(256, 16, 7.1, 6.9,
                                                         totalCount, 23);

  auto values = std::array<std::uint64_t, totalCount>();
  std::ranges::copy(src, values.begin());

  auto firsts = std::ranges::views::take(values, values.size() - 1);
  auto seconds = std::ranges::views::drop(values, 1);

  std::map<std::pair<std::uint64_t, std::uint64_t>, std::size_t> counts;

  for (auto i : std::ranges::iota_view(0, 99999)) {
    ++counts[std::make_pair(firsts[i], seconds[i])];
  }

  const auto pLogPs =
      counts | std::views::transform([](const auto& entry) {
        auto [ord, count] = entry;
        const double countDouble = static_cast<double>(count);
        const double p = countDouble / (static_cast<double>(totalCount) - 1.0);
        return -p * std::log2(p);
      });

  double entropyEstimation =
      std::accumulate(pLogPs.begin(), pLogPs.end(), 0.) - 7.1;

  EXPECT_TRUE((entropyEstimation - 6.9) / 6.9 < 0.1);
}

// NOLINTEND(cppcoreguidelines-owning-memory,
// cppcoreguidelines-avoid-magic-numbers)
