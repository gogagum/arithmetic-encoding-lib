#include <gtest/gtest.h>

#include <ael/esc/dictionary/adaptive_a_dictionary.hpp>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cert-err58-cpp)

TEST(EscAdaptiveADictionary, Construct) {
  auto dict = ael::esc::dict::AdaptiveADictionary(42);
}

TEST(EscAdaptiveADictionary, FirstGet) {
  auto dict = ael::esc::dict::AdaptiveADictionary(42);

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
  auto dict = ael::esc::dict::AdaptiveADictionary(42);

  const auto stats = dict.getProbabilityStats(3);
  const auto stats2 = dict.getProbabilityStats(3);

  EXPECT_EQ(stats2.size(), 1);

  EXPECT_EQ(stats2[0].low, 0);
  EXPECT_EQ(stats2[0].high, 1);
  EXPECT_EQ(stats2[0].total, 2);
}

TEST(EscAdaptiveADDictionary, TwoInitialStatsGet) {
  auto dict = ael::esc::dict::AdaptiveADictionary(42);

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
  auto dict = ael::esc::dict::AdaptiveADictionary(42);

  auto ord = dict.getWordOrd(0);

  EXPECT_EQ(ord, 42);
}

TEST(EscAdaptiveADDictionary, getTotalWordCntOnInit) {
  auto dict = ael::esc::dict::AdaptiveADictionary(42);

  auto totalCnt = dict.getTotalWordsCnt();

  EXPECT_EQ(totalCnt, 1);
}

TEST(EscAdaptiveADictionary, Example) {
  auto dict = ael::esc::dict::AdaptiveADictionary(256);
  const auto stats0 = dict.getProbabilityStats('I');
  EXPECT_EQ(stats0.size(), 2);
  EXPECT_EQ(stats0[0].high - stats0[0].low, 1);
  EXPECT_EQ(stats0[0].total, 1);
  EXPECT_EQ(stats0[1].high - stats0[1].low, 1);
  EXPECT_EQ(stats0[1].total, 256);
  const auto stats1 = dict.getProbabilityStats('F');
  EXPECT_EQ(stats1.size(), 2);
  EXPECT_EQ(stats1[0].high - stats1[0].low, 1);
  EXPECT_EQ(stats1[0].total, 2);
  EXPECT_EQ(stats1[1].high - stats1[1].low, 1);
  EXPECT_EQ(stats1[1].total, 255);
  const auto stats2 = dict.getProbabilityStats('_');
  EXPECT_EQ(stats2.size(), 2);
  EXPECT_EQ(stats2[0].high - stats2[0].low, 1);
  EXPECT_EQ(stats2[0].total, 3);
  EXPECT_EQ(stats2[1].high - stats2[1].low, 1);
  EXPECT_EQ(stats2[1].total, 254);
  const auto stats3 = dict.getProbabilityStats('W');
  EXPECT_EQ(stats3.size(), 2);
  EXPECT_EQ(stats3[0].high - stats3[0].low, 1);
  EXPECT_EQ(stats3[0].total, 4);
  EXPECT_EQ(stats3[1].high - stats3[1].low, 1);
  EXPECT_EQ(stats3[1].total, 253);
  const auto stats4 = dict.getProbabilityStats('E');
  EXPECT_EQ(stats4.size(), 2);  
  EXPECT_EQ(stats4[0].high - stats4[0].low, 1);
  EXPECT_EQ(stats4[0].total, 5);
  EXPECT_EQ(stats4[1].high - stats4[1].low, 1);
  EXPECT_EQ(stats4[1].total, 252);
  const auto stats5 = dict.getProbabilityStats('_');
  EXPECT_EQ(stats5.size(), 1);
  EXPECT_EQ(stats5[0].high - stats5[0].low, 1);
  EXPECT_EQ(stats5[0].total, 6);
  const auto stats6 = dict.getProbabilityStats('C');
  EXPECT_EQ(stats6.size(), 2);
  EXPECT_EQ(stats6[0].high - stats6[0].low, 1);
  EXPECT_EQ(stats6[0].total, 7);
  EXPECT_EQ(stats6[1].high - stats6[1].low, 1);
  EXPECT_EQ(stats6[1].total, 251);
  const auto stats7 = dict.getProbabilityStats('A');
  EXPECT_EQ(stats7.size(), 2);
  EXPECT_EQ(stats7[0].high - stats7[0].low, 1);
  EXPECT_EQ(stats7[0].total, 8);
  EXPECT_EQ(stats7[1].high - stats7[1].low, 1);
  EXPECT_EQ(stats7[1].total, 250);
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cert-err58-cpp)
