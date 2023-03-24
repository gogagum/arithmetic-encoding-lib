#include <gtest/gtest.h>

#include <ael/dictionary/adaptive_d_contextual_dictionary.hpp>

using ael::dict::AdaptiveDContextualDictionary;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(AdaptiveDContextualDictionary, Construct) {
    const auto dict = AdaptiveDContextualDictionary(3, 3, 5);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDContextualDictionary, WordsCountAfterInit) {
    const auto dict = AdaptiveDContextualDictionary(8, 8, 3);
    EXPECT_EQ(dict.getTotalWordsCnt(), 256);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDContextualDictionary, GetInitProbabilityStatsOrd0) {
    auto dict = AdaptiveDContextualDictionary(8, 3, 8);
    const auto [low, high, totalWordsCount] = dict.getProbabilityStats(0);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
    EXPECT_EQ(totalWordsCount, 256);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDContextualDictionary, GetInitProbabilityStatsOrdEnd) {
    auto dict = AdaptiveDContextualDictionary(8, 3, 8);
    const auto [low, high, totalWordsCount] = dict.getProbabilityStats(255);
    EXPECT_EQ(low, 255);
    EXPECT_EQ(high, 256);
    EXPECT_EQ(totalWordsCount, 256);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDContextualDictionary, GetInitProbabilityStatsOrdCenter) {
    auto dict = AdaptiveDContextualDictionary(8, 3, 8);
    const auto [low, high, totalWordsCount] = dict.getProbabilityStats(42);
    EXPECT_EQ(low, 42);
    EXPECT_EQ(high, 43);
    EXPECT_EQ(totalWordsCount, 256);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDContextualDictionary, DoubleGetStatsSame) {
    auto dict = AdaptiveDContextualDictionary(8, 3, 8);
    [[maybe_unused]] const auto _stats = dict.getProbabilityStats(42);
    const auto [low1, high1, totalWordsCount] = dict.getProbabilityStats(42);
    EXPECT_EQ(low1, 42);
    EXPECT_EQ(high1, 42 + 255);
    EXPECT_EQ(totalWordsCount, 255 * 2);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDContextualDictionary, DoubleGetStatsDifferent) {
    auto dict = AdaptiveDContextualDictionary(8, 3, 8);
    [[maybe_unused]] const auto _stats = dict.getProbabilityStats(42);
    const auto [low1, high1, totalWordsCount] = dict.getProbabilityStats(45);
    EXPECT_EQ(low1, 255 + 45 - 1);
    EXPECT_EQ(high1, 255 + 46 - 1);
    EXPECT_EQ(totalWordsCount, 255 * 2);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDContextualDictionary, DoubleGetStatsDifferent2) {
    auto dict = AdaptiveDContextualDictionary(8, 3, 8);
    [[maybe_unused]] const auto _stats = dict.getProbabilityStats(42);
    const auto [low1, high1, totalWordsCount] = dict.getProbabilityStats(43);
    EXPECT_EQ(low1, 255 + 43 - 1);
    EXPECT_EQ(high1, 255 + 44 - 1);
    EXPECT_EQ(totalWordsCount, 255 * 2);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDContextualDictionary, Example) {
    auto dict = AdaptiveDContextualDictionary(8, 3, 8);
    const auto [low0, high0, total0] = dict.getProbabilityStats('I');
    EXPECT_EQ(high0 - low0, 1);
    EXPECT_EQ(total0, 256);
    const auto [low1, high1, total1] = dict.getProbabilityStats('F');
    EXPECT_EQ(high1 - low1, 1);
    EXPECT_EQ(total1, 255 * 2);
    const auto [low2, high2, total2] = dict.getProbabilityStats('_');
    EXPECT_EQ(high2 - low2, 2);
    EXPECT_EQ(total2, 254 * 4);
    const auto [low3, high3, total3] = dict.getProbabilityStats('W');
    EXPECT_EQ(high3 - low3, 3);
    EXPECT_EQ(total3, 253 * 6);
    const auto [low4, high4, total4] = dict.getProbabilityStats('E');
    EXPECT_EQ(high4 - low4, 4);
    EXPECT_EQ(total4, 252 * 8);
    const auto [low5, high5, total5] = dict.getProbabilityStats('_');
    EXPECT_EQ(high5 - low5, 251);
    EXPECT_EQ(total5, 251 * 10);
    const auto [low6, high6, total6] = dict.getProbabilityStats('C');
    EXPECT_EQ(high6 - low6, 1);
    EXPECT_EQ(total6, 255 * 2);
    const auto [low7, high7, total7] = dict.getProbabilityStats('A');
    EXPECT_EQ(high7 - low7, 6);
    EXPECT_EQ(total7, 250 * 14);
}

