#include <gtest/gtest.h>

#include <ael/esc/dictionary/ppmd_dictionary.hpp>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cert-err58-cpp)

using ael::esc::dict::PPMDDictionary;

TEST(EscPPMDDictionary, Construct) {
  auto dict = PPMDDictionary({42, 3});
}

TEST(EscPPMDDictionary, FirstGet) {
  auto dict = PPMDDictionary({42, 3});

  const auto statsSeq = dict.getProbabilityStats(3);

  EXPECT_EQ(statsSeq.size(), 2);

  EXPECT_EQ(statsSeq[0].low, 0);
  EXPECT_EQ(statsSeq[0].high, 1);
  EXPECT_EQ(statsSeq[0].total, 1);

  EXPECT_EQ(statsSeq[1].low, 3);
  EXPECT_EQ(statsSeq[1].high, 4);
  EXPECT_EQ(statsSeq[1].total, 42);
}

TEST(EscPPMDDictionary, TwoInitialStatsGet) {
  auto dict = PPMDDictionary({42, 3});

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

TEST(EscPPMDDictionary, GetWordOrdOnInit) {
  auto dict = PPMDDictionary({42, 3});

  auto ord = dict.getWordOrd(0);

  EXPECT_EQ(ord, 42);
}

TEST(EscPPMDDictionary, getTotalWordCntOnInit) {
  auto dict = PPMDDictionary({42, 3});

  auto totalCnt = dict.getTotalWordsCnt();

  EXPECT_EQ(totalCnt, 1);
}

TEST(EscPPMDDictionary, GetStatsOnStartCenter) {
  auto dict = PPMDDictionary({8, 3});
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

TEST(EscPPMDDictionary, GetStatsOnStartEnd) {
  auto dict = PPMDDictionary({8, 3});
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

TEST(EscPPMDDictionary, GetStatsOnStartBegin) {
  auto dict = PPMDDictionary({8, 3});
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

TEST(EscPPMDDictionary, GetStatsAfterUpdate) {
  auto dict = PPMDDictionary({8, 3});
  [[maybe_unused]] auto stats0 = dict.getProbabilityStats(0);
  auto stats1 = dict.getProbabilityStats(0);
  EXPECT_EQ(stats1.size(), 1);
  EXPECT_EQ(stats1[0].low, 0);
  EXPECT_EQ(stats1[0].high, 1);
  EXPECT_EQ(stats1[0].total, 2);
  // Unuform probability. Each letter 1/8.
}

TEST(EscPPMDDictionary, GetStatsAfterIncreaseOneUpdateOtherCenterCenter) {
  auto dict = PPMDDictionary({8, 3});
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

TEST(EscPPMDDictionary, GetStatsAfterIncreaseOneUpdateOtherCenterBegin) {
  auto dict = PPMDDictionary({8, 3});
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

TEST(EscPPMDDictionary, GetStatsAfterIncreaseOneUpdateOtherCenterEnd) {
  auto dict = PPMDDictionary({8, 3});
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

TEST(EscPPMDDictionary, GetStatsAfterIncreaseOneUpdateOtherBeginCenter) {
  auto dict = PPMDDictionary({8, 3});
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

TEST(EscPPMDDictionary, Example) {
  auto dict = PPMDDictionary({256, 5});
  
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
    EXPECT_EQ(stats[0].high - stats[0].low, 2);
    EXPECT_EQ(stats[0].total, 4);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 254);
  }

  {
    const auto stats = dict.getProbabilityStats('W');
    EXPECT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].high - stats[0].low, 3);
    EXPECT_EQ(stats[0].total, 6);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 253);
  }

  {
    const auto stats = dict.getProbabilityStats('E');
    EXPECT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].high - stats[0].low, 4);
    EXPECT_EQ(stats[0].total, 8);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 252);
  }

  {
    const auto stats = dict.getProbabilityStats('_');
    EXPECT_EQ(stats.size(), 1);
    EXPECT_EQ(stats[0].high - stats[0].low, 1);
    EXPECT_EQ(stats[0].total, 10);
  }

  {
    const auto stats = dict.getProbabilityStats('C');
    EXPECT_EQ(stats.size(), 3);
    EXPECT_EQ(stats[0].high - stats[0].low, 1);
    EXPECT_EQ(stats[0].total, 2);
    EXPECT_EQ(stats[1].high - stats[1].low, 5);
    EXPECT_EQ(stats[1].total, 12);
    EXPECT_EQ(stats[2].high - stats[2].low, 1);
    EXPECT_EQ(stats[2].total, 251);
  }

  {
    const auto stats = dict.getProbabilityStats('A');
    EXPECT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].high - stats[0].low, 6);
    EXPECT_EQ(stats[0].total, 14);
    EXPECT_EQ(stats[1].high - stats[1].low, 1);
    EXPECT_EQ(stats[1].total, 250);
  }
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cert-err58-cpp)
