#include <gtest/gtest.h>

#include <array>
#include <cstddef>

#include <ael/dictionary/adaptive_a_dictionary.hpp>

using ael::dict::AdaptiveADictionary;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, Construct) {
    auto dict = AdaptiveADictionary(256 * 256);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStats) {
    auto dict = AdaptiveADictionary(256 * 256);
    [[maybe_unused]] const auto [low, high, total] =
            dict.getProbabilityStats(1);
}

//----------------------------------------------------------------------------//
TEST(AdapriveADictionary, GetStatsOnStartCenter) {
    auto dict = AdaptiveADictionary(8);
    const auto [low, high, total] = dict.getProbabilityStats(6);
    EXPECT_EQ(low, 6);
    EXPECT_EQ(high, 7);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsOnStartEnd) {
    auto dict = AdaptiveADictionary(8);
    const auto [low, high, total] = dict.getProbabilityStats(7);
    EXPECT_EQ(low, 7);
    EXPECT_EQ(high, 8);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsOnStartBegin) {
    auto dict = AdaptiveADictionary(8);
    const auto [low, high, total] = dict.getProbabilityStats(0);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterUpdate) {
    auto dict = AdaptiveADictionary(8);
    [[maybe_unused]] auto stats0 = dict.getProbabilityStats(0);
    auto [low, high, total] = dict.getProbabilityStats(0);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 7);
    EXPECT_EQ(total, 14);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterCenter) {
    auto dict = AdaptiveADictionary(8);
    [[maybe_unused]] auto stats0 = dict.getProbabilityStats(2);
    const auto [low, high, total] = dict.getProbabilityStats(5);
    EXPECT_EQ(low, 11);
    EXPECT_EQ(high, 12);
    EXPECT_EQ(total, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterBegin) {
    auto dict = AdaptiveADictionary(8);
    [[maybe_unused]] auto stats0 = dict.getProbabilityStats(2);
    const auto [low, high, total] = dict.getProbabilityStats(0);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
    EXPECT_EQ(total, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterEnd) {
    auto dict = AdaptiveADictionary(8);
    [[maybe_unused]] auto _stats0 = dict.getProbabilityStats(2);
    auto [low, high, total] = dict.getProbabilityStats(7);
    EXPECT_EQ(low, 13);
    EXPECT_EQ(high, 14);
    EXPECT_EQ(total, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherBeginCenter) {
    auto dict = AdaptiveADictionary(8);
    [[maybe_unused]] const auto _stats0 = dict.getProbabilityStats(0);
    const auto [low, high, total] = dict.getProbabilityStats(5);
    EXPECT_EQ(low, 11);
    EXPECT_EQ(high, 12);
    EXPECT_EQ(total, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, Example) {
    auto dict = AdaptiveADictionary(256);
    const auto [low0, high0, total0] = dict.getProbabilityStats('I');
    EXPECT_EQ(high0 - low0, 1);
    EXPECT_EQ(total0, 256);
    const auto [low1, high1, total1] = dict.getProbabilityStats('F');
    EXPECT_EQ(high1 - low1, 1);
    EXPECT_EQ(total1, 255 * 2);
    const auto [low2, high2, total2] = dict.getProbabilityStats('_');
    EXPECT_EQ(high2 - low2, 1);
    EXPECT_EQ(total2, 254 * 3);
    const auto [low3, high3, total3] = dict.getProbabilityStats('W');
    EXPECT_EQ(high3 - low3, 1);
    EXPECT_EQ(total3, 253 * 4);
    const auto [low4, high4, total4] = dict.getProbabilityStats('E');
    EXPECT_EQ(high4 - low4, 1);
    EXPECT_EQ(total4, 252 * 5);
    const auto [low5, high5, total5] = dict.getProbabilityStats('_');
    EXPECT_EQ(high5 - low5, 251);
    EXPECT_EQ(total5, 251 * 6);
    const auto [low6, high6, total6] = dict.getProbabilityStats('C');
    EXPECT_EQ(high6 - low6, 1);
    EXPECT_EQ(total6, 251 * 7);
    const auto [low7, high7, total7] = dict.getProbabilityStats('A');
    EXPECT_EQ(high7 - low7, 1);
    EXPECT_EQ(total7, 250 * 8);
}
