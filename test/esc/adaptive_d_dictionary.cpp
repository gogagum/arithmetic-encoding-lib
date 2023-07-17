#include <gtest/gtest.h>

#include <ael/esc/dictionary/adaptive_d_dictionary.hpp>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cert-err58-cpp)

using ael::esc::dict::AdaptiveDDictionary;

TEST(EscAdaptiveDDictionary, Construct) {
  auto dict = AdaptiveDDictionary(42);
}

TEST(EscAdaptiveDDictionary, FirstGet) {
  auto dict = AdaptiveDDictionary(42);

  const auto stats = dict.getProbabilityStats(3);

  EXPECT_EQ(stats.size(), 2);

  EXPECT_EQ(stats[0].low, 0);
  EXPECT_EQ(stats[0].high, 1);
  EXPECT_EQ(stats[0].total, 1);

  EXPECT_EQ(stats[1].low, 3);
  EXPECT_EQ(stats[1].high, 4);
  EXPECT_EQ(stats[1].total, 42);
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cert-err58-cpp)
