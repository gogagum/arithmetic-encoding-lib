#include <gtest/gtest.h>

#include <ael/dictionary/adaptive_d_dictionary.hpp>

using ael::dict::AdaptiveDDictionary;

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

TEST(AdaptiveDDictionary, Construct) {
  auto dict = AdaptiveDDictionary(256ul * 256u);
}

TEST(AdaptiveDDictionary, GetStats) {
  auto dict = AdaptiveDDictionary(256ul * 256u);
  [[maybe_unused]] const auto _stats = dict.getProbabilityStats(1);
}

TEST(AdaptiveDDictionary, GetStatsOnStartCenter) {
  auto dict = AdaptiveDDictionary(8);
  const auto [low, high, total] = dict.getProbabilityStats(6);
  EXPECT_EQ(low, 6);
  EXPECT_EQ(high, 7);
  EXPECT_EQ(total, 8);
  // Uniform probability. Each letter 1/8.
}

TEST(AdaptiveDDictionary, GetStatsOnStartEnd) {
  auto dict = AdaptiveDDictionary(8);
  const auto [low, high, total] = dict.getProbabilityStats(7);
  EXPECT_EQ(low, 7);
  EXPECT_EQ(high, 8);
  EXPECT_EQ(total, 8);
  // Uniform probability. Each letter 1/8.
}

TEST(AdaptiveDDictionary, GetStatsOnStartBegin) {
  auto dict = AdaptiveDDictionary(8);
  const auto [low, high, total] = dict.getProbabilityStats(0);
  EXPECT_EQ(low, 0);
  EXPECT_EQ(high, 1);
  EXPECT_EQ(total, 8);
  // Uniform probability. Each letter 1/8.
}

TEST(AdaptiveDDictionary, Example) {
  auto dict = AdaptiveDDictionary(256);

  {
    const auto [low, high, total] = dict.getProbabilityStats('I');
    EXPECT_EQ(high - low, 1);
    EXPECT_EQ(total, 256);
  }

  {
    const auto [low, high, total] = dict.getProbabilityStats('F');
    EXPECT_EQ(high - low, 1);
    EXPECT_EQ(total, 255 * 2);
  }

  {
    const auto [low, high, total] = dict.getProbabilityStats('_');
    EXPECT_EQ(high - low, 2);
    EXPECT_EQ(total, 254 * 4);
  }

  {
    const auto [low, high, total] = dict.getProbabilityStats('W');
    EXPECT_EQ(high - low, 3);
    EXPECT_EQ(total, 253 * 6);
  }

  {
    const auto [low, high, total] = dict.getProbabilityStats('E');
    EXPECT_EQ(high - low, 4);
    EXPECT_EQ(total, 252 * 8);
  }

  {
    const auto [low, high, total] = dict.getProbabilityStats('_');
    EXPECT_EQ(high - low, 251);
    EXPECT_EQ(total, 251 * 10);
  }

  {
    const auto [low, high, total] = dict.getProbabilityStats('C');
    EXPECT_EQ(high - low, 5);
    EXPECT_EQ(total, 251 * 12);
  }

  {
    const auto [low, high, total] = dict.getProbabilityStats('A');
    EXPECT_EQ(high - low, 6);
    EXPECT_EQ(total, 250 * 14);
  }
}

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
