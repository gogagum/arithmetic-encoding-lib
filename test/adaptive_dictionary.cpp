#include <gtest/gtest.h>

#include <array>
#include <cstddef>

#include <ael/dictionary/adaptive_dictionary.hpp>

using ael::dict::AdaptiveDictionary;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, ConstructBytesWord) {
    auto dict = AdaptiveDictionary(256 * 2, 1);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, ConstructBitsWord) {
    auto dict = AdaptiveDictionary(256 * 2, 2);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, ConstructIntWord) {
    auto dict = AdaptiveDictionary(8, 3);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, LowerAfterConstruct) {
    auto dict = AdaptiveDictionary(256, 3);
    const auto [low, _0, _1] = dict.getProbabilityStats(37);
    EXPECT_EQ(low, 37);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, LowerZeroAfterConstruct) {
    auto dict = AdaptiveDictionary(256, 2);
    const auto [low, _0, _1] = dict.getProbabilityStats(0);
    EXPECT_EQ(low, 0);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, HigherZeroAfterConstruct) {
    auto dict = AdaptiveDictionary(256, 2);
    const auto [_0, high, _1] = dict.getProbabilityStats(42);
    EXPECT_EQ(high, 43);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, Increase) {
    auto dict = AdaptiveDictionary(256, 1);
    [[maybe_unused]] const auto stats = dict.getProbabilityStats(12);
    const auto [low, _0, _1] = dict.getProbabilityStats(42);
    EXPECT_EQ(low, 43);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, IncreaseExactTheSame) {
    auto dict = AdaptiveDictionary(256, 1);

    const auto [low1, high1, _1] = dict.getProbabilityStats(12);
    EXPECT_EQ(low1, 12);  // before first increase
    EXPECT_EQ(high1, 13); // before first increase
    const auto [low2, high2, _2] = dict.getProbabilityStats(12);
    EXPECT_EQ(low2, 12);  // after first increase
    EXPECT_EQ(high2, 14); // after first increase
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, IncreaseZero) {
    auto dict = AdaptiveDictionary(256, 1);

    const auto [low1, high1, _1] = dict.getProbabilityStats(0);
    EXPECT_EQ(low1, 0);   // before first increase
    EXPECT_EQ(high1, 1);  // before first increase
    const auto [low2, high2, _2] = dict.getProbabilityStats(0);
    EXPECT_EQ(low2, 0);   // after first increase
    EXPECT_EQ(high2, 2);  // after first increase
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, GetWordOnStart) {
    auto dict = AdaptiveDictionary(256, 1);
    const auto ord = dict.getWordOrd(42);
    EXPECT_EQ(ord, 42);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, GetWordAfterUpdate) {
    auto dict = AdaptiveDictionary(256, 1);
    [[maybe_unused]] const auto stats =
            dict.getProbabilityStats(42);
    EXPECT_EQ(dict.getWordOrd(42), 42);
    EXPECT_EQ(dict.getWordOrd(43), 42);
}
