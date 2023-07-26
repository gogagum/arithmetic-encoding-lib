#include <gtest/gtest.h>

#include <ael/esc/dictionary/adaptive_a_dictionary.hpp>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cert-err58-cpp)

using ael::esc::dict::AdaptiveADictionary;

TEST(EscAdaptiveADictionary, Construct) {
  auto dict = AdaptiveADictionary(42);
}

TEST(EscAdaptiveADictionary, FirstGet) {
  auto dict = AdaptiveADictionary(42);

  const auto stats = dict.getProbabilityStats(3);

  EXPECT_EQ(stats.size(), 2);

  EXPECT_EQ(stats[0].low, 0);
  EXPECT_EQ(stats[0].high, 1);
  EXPECT_EQ(stats[0].total, 1);

  EXPECT_EQ(stats[1].low, 3);
  EXPECT_EQ(stats[1].high, 4);
  EXPECT_EQ(stats[1].total, 42);
}

TEST(EscAdaptiveADictionary, RepeatedGetOnInitial) {
  auto dict = AdaptiveADictionary(42);

  const auto stats = dict.getProbabilityStats(3);
  const auto stats2 = dict.getProbabilityStats(3);

  EXPECT_EQ(stats2.size(), 1);

  EXPECT_EQ(stats2[0].low, 0);
  EXPECT_EQ(stats2[0].high, 1);
  EXPECT_EQ(stats2[0].total, 2);
}

TEST(EscAdaptiveADDictionary, TwoInitialStatsGet) {
  auto dict = AdaptiveADictionary(42);

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

TEST(EscAdaptiveADDictionary, GetWordOrdOnInit) {
  auto dict = AdaptiveADictionary(42);

  auto ord = dict.getWordOrd(0);

  EXPECT_EQ(ord, 42);
}

TEST(EscAdaptiveADDictionary, getTotalWordCntOnInit) {
  auto dict = AdaptiveADictionary(42);

  auto totalCnt = dict.getTotalWordsCnt();

  EXPECT_EQ(totalCnt, 1);
}

TEST(EscAdapriveADictionary, GetStatsOnStartCenter) {
  auto dict = AdaptiveADictionary(8);
  const auto stats = dict.getProbabilityStats(6);
  EXPECT_EQ(stats.size(), 2);

  EXPECT_EQ(stats[0].low, 0);
  EXPECT_EQ(stats[0].high, 1);
  EXPECT_EQ(stats[0].total, 1);

  EXPECT_EQ(stats[1].low, 6);
  EXPECT_EQ(stats[1].high, 7);
  EXPECT_EQ(stats[1].total, 8);
  // Unuform probability. Each letter 1/8.
}

TEST(EscAdaptiveADictionary, GetStatsOnStartEnd) {
  auto dict = AdaptiveADictionary(8);
  const auto stats = dict.getProbabilityStats(7);
  EXPECT_EQ(stats.size(), 2);

  EXPECT_EQ(stats[0].low, 0);
  EXPECT_EQ(stats[0].high, 1);
  EXPECT_EQ(stats[0].total, 1);

  EXPECT_EQ(stats[1].low, 7);
  EXPECT_EQ(stats[1].high, 8);
  EXPECT_EQ(stats[1].total, 8);
  // Unuform probability. Each letter 1/8.
}

TEST(EscAdaptiveADictionary, GetStatsOnStartBegin) {
  auto dict = AdaptiveADictionary(8);
  const auto stats = dict.getProbabilityStats(0);
  EXPECT_EQ(stats.size(), 2);

  EXPECT_EQ(stats[0].low, 0);
  EXPECT_EQ(stats[0].high, 1);
  EXPECT_EQ(stats[0].total, 1);

  EXPECT_EQ(stats[1].low, 0);
  EXPECT_EQ(stats[1].high, 1);
  EXPECT_EQ(stats[1].total, 8);
  // Unuform probability. Each letter 1/8.
}

TEST(EscAdaptiveADictionary, GetStatsAfterUpdate) {
  auto dict = AdaptiveADictionary(8);
  [[maybe_unused]] auto stats0 = dict.getProbabilityStats(0);
  auto stats1 = dict.getProbabilityStats(0);
  EXPECT_EQ(stats1.size(), 1);
  EXPECT_EQ(stats1[0].low, 0);
  EXPECT_EQ(stats1[0].high, 1);
  EXPECT_EQ(stats1[0].total, 2);
  // Unuform probability. Each letter 1/8.
}

TEST(EscAdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterCenter) {
  auto dict = AdaptiveADictionary(8);
  [[maybe_unused]] auto stats0 = dict.getProbabilityStats(2);
  const auto stats1 = dict.getProbabilityStats(5);
  EXPECT_EQ(stats1.size(), 2);

  EXPECT_EQ(stats1[0].low, 1);
  EXPECT_EQ(stats1[0].high, 2);
  EXPECT_EQ(stats1[0].total, 2);
  EXPECT_EQ(stats1[1].low, 4);
  EXPECT_EQ(stats1[1].high, 5);
  EXPECT_EQ(stats1[1].total, 7);
}

TEST(EscAdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterBegin) {
  auto dict = AdaptiveADictionary(8);
  [[maybe_unused]] auto stats0 = dict.getProbabilityStats(2);
  const auto stats1 = dict.getProbabilityStats(0);
  EXPECT_EQ(stats1.size(), 2);

  EXPECT_EQ(stats1[0].low, 1);
  EXPECT_EQ(stats1[0].high, 2);
  EXPECT_EQ(stats1[0].total, 2);
  EXPECT_EQ(stats1[1].low, 0);
  EXPECT_EQ(stats1[1].high, 1);
  EXPECT_EQ(stats1[1].total, 7);
}

TEST(EscAdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterEnd) {
  auto dict = AdaptiveADictionary(8);
  [[maybe_unused]] auto _stats0 = dict.getProbabilityStats(2);
  const auto stats1 = dict.getProbabilityStats(7);
  EXPECT_EQ(stats1.size(), 2);
  EXPECT_EQ(stats1[0].low, 1);
  EXPECT_EQ(stats1[0].high, 2);
  EXPECT_EQ(stats1[0].total, 2);
  EXPECT_EQ(stats1[1].low, 6);
  EXPECT_EQ(stats1[1].high, 7);
  EXPECT_EQ(stats1[1].total, 7);
}

TEST(EscAdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherBeginCenter) {
  auto dict = AdaptiveADictionary(8);
  [[maybe_unused]] const auto _stats0 = dict.getProbabilityStats(0);
  const auto stats1 = dict.getProbabilityStats(5);
  EXPECT_EQ(stats1.size(), 2);
  EXPECT_EQ(stats1[0].low, 1);
  EXPECT_EQ(stats1[0].high, 2);
  EXPECT_EQ(stats1[0].total, 2);
  EXPECT_EQ(stats1[1].low, 4);
  EXPECT_EQ(stats1[1].high, 5);
  EXPECT_EQ(stats1[1].total, 7);
}

TEST(EscAdaptiveADictionary, Example) {
  auto dict = AdaptiveADictionary(256);
  
  {
    const auto stats = dict.getProbabilityStats('I');
    EXPECT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].high - stats[0].low, 1);
    EXPECT_EQ(stats[0].total, 1);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 256);
  }

  {
    const auto stats = dict.getProbabilityStats('F');
    EXPECT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].high - stats[0].low, 1);
    EXPECT_EQ(stats[0].total, 2);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 255);
  }

  {
    const auto stats = dict.getProbabilityStats('_');
    EXPECT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].high - stats[0].low, 1);
    EXPECT_EQ(stats[0].total, 3);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 254);
  }

  {
    const auto stats = dict.getProbabilityStats('W');
    EXPECT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].high - stats[0].low, 1);
    EXPECT_EQ(stats[0].total, 4);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 253);
  }

  {
    const auto stats = dict.getProbabilityStats('E');
    EXPECT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].high - stats[0].low, 1);
    EXPECT_EQ(stats[0].total, 5);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 252);
  }

  {
    const auto stats = dict.getProbabilityStats('_');
    EXPECT_EQ(stats.size(), 1);
    EXPECT_EQ(stats[0].high - stats[0].low, 1);
    EXPECT_EQ(stats[0].total, 6);
  }

  {
    const auto stats = dict.getProbabilityStats('C');
    EXPECT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].high - stats[0].low, 1);
    EXPECT_EQ(stats[0].total, 7);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 251);
  }

  {
    const auto stats = dict.getProbabilityStats('A');
    EXPECT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].high - stats[0].low, 1);
    EXPECT_EQ(stats[0].total, 8);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 250);
  }
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cert-err58-cpp)
