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

TEST(EscAdaptiveDDictionary, RepeatedGetOnInitial) {
  auto dict = AdaptiveDDictionary(42);

  const auto stats = dict.getProbabilityStats(3);
  const auto stats2 = dict.getProbabilityStats(3);

  EXPECT_EQ(stats2.size(), 1);

  EXPECT_EQ(stats2[0].low, 0);
  EXPECT_EQ(stats2[0].high, 1);
  EXPECT_EQ(stats2[0].total, 2);
}

TEST(EscAdaptiveDDictionary, TwoInitialStatsGet) {
  auto dict = AdaptiveDDictionary(42);

  [[maybe_unused]] const auto stats0 = dict.getProbabilityStats(3);
  const auto stats1 = dict.getProbabilityStats(37);

  EXPECT_EQ(stats1.size(), 2);

  EXPECT_EQ(stats1[0].low, 1);
  EXPECT_EQ(stats1[0].high, 2);
  EXPECT_EQ(stats1[0].total, 2);

  EXPECT_EQ(stats1[1].low, 36);
  EXPECT_EQ(stats1[1].high, 37);
  EXPECT_EQ(stats1[1].total, 41);
}

TEST(EscAdaptiveDDictionary, GetWordOrdOnInit) {
  auto dict = AdaptiveDDictionary(42);

  auto ord = dict.getWordOrd(0);

  EXPECT_EQ(ord, 42);
}

TEST(EscAdaptiveDDictionary, getTotalWordCntOnInit) {
  auto dict = AdaptiveDDictionary(42);

  auto totalCnt = dict.getTotalWordsCnt();

  EXPECT_EQ(totalCnt, 1);
}

TEST(EscAdaptiveDDictionary, GetStatsOnStartCenter) {
  auto dict = AdaptiveDDictionary(8);
  const auto stats = dict.getProbabilityStats(6);
  EXPECT_EQ(stats.size(), 2);

  EXPECT_EQ(stats[0].low, 0);
  EXPECT_EQ(stats[0].high, 1);
  EXPECT_EQ(stats[0].total, 1);

  EXPECT_EQ(stats[1].low, 6);
  EXPECT_EQ(stats[1].high, 7);
  EXPECT_EQ(stats[1].total, 8);
  // Uniform probability. Each letter 1/8.
}

TEST(EscAdaptiveDDictionary, GetStatsOnStartEnd) {
  auto dict = AdaptiveDDictionary(8);
  const auto stats = dict.getProbabilityStats(7);
  EXPECT_EQ(stats.size(), 2);

  EXPECT_EQ(stats[0].low, 0);
  EXPECT_EQ(stats[0].high, 1);
  EXPECT_EQ(stats[0].total, 1);

  EXPECT_EQ(stats[1].low, 7);
  EXPECT_EQ(stats[1].high, 8);
  EXPECT_EQ(stats[1].total, 8);
  // Uniform probability. Each letter 1/8.
}

TEST(EscAdaptiveDDictionary, GetStatsOnStartBegin) {
  auto dict = AdaptiveDDictionary(8);
  const auto stats = dict.getProbabilityStats(0);
  EXPECT_EQ(stats.size(), 2);

  EXPECT_EQ(stats[0].low, 0);
  EXPECT_EQ(stats[0].high, 1);
  EXPECT_EQ(stats[0].total, 1);

  EXPECT_EQ(stats[1].low, 0);
  EXPECT_EQ(stats[1].high, 1);
  EXPECT_EQ(stats[1].total, 8);
  // Uniform probability. Each letter 1/8.
}

TEST(EscAdaptiveDDictionary, Example) {
  auto dict = AdaptiveDDictionary(256);
  
  {
    const auto stats = dict.getProbabilityStats('I');
    EXPECT_EQ(stats.size(), 2);
    const auto [low, high, total] = stats[1];
    EXPECT_EQ(high - low, 1);
    EXPECT_EQ(total, 256);
  }

  {
    const auto stats = dict.getProbabilityStats('F');
    EXPECT_EQ(stats.size(), 2);
    const auto [low, high, total] = stats[1];
    EXPECT_EQ(high - low, 1);
    EXPECT_EQ(total, 255);
  }

  {
    const auto stats = dict.getProbabilityStats('_');
    EXPECT_EQ(stats.size(), 2);
    const auto [low, high, total] = stats[1];
    EXPECT_EQ(high - low, 1);
    EXPECT_EQ(total, 254);
  }

  {
    const auto stats = dict.getProbabilityStats('W');
    EXPECT_EQ(stats.size(), 2);
    const auto [low, high, total] = stats[1];
    EXPECT_EQ(high - low, 1);
    EXPECT_EQ(total, 253);
  }

  {
    const auto stats = dict.getProbabilityStats('E');
    EXPECT_EQ(stats.size(), 2);
    const auto [low, high, total] = stats[1];
    EXPECT_EQ(high - low, 1);
    EXPECT_EQ(total, 252);
  }

  {
    const auto stats = dict.getProbabilityStats('_');
    EXPECT_EQ(stats.size(), 1);
    const auto [low, high, total] = stats[0];
    EXPECT_EQ(high - low, 1);
    EXPECT_EQ(total, 10);
  }

  {
    const auto stats = dict.getProbabilityStats('C');
    EXPECT_EQ(stats.size(), 2);
    const auto [low, high, total] = stats[1];
    EXPECT_EQ(high - low, 1);
    EXPECT_EQ(total, 251);
  }

  {
    const auto stats = dict.getProbabilityStats('A');
    EXPECT_EQ(stats.size(), 2);
    const auto [low, high, total] = stats[1];
    EXPECT_EQ(high - low, 1);
    EXPECT_EQ(total, 250);
  }
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cert-err58-cpp)
