#include <gtest/gtest.h>

#include <array>
#include <cstddef>

#include <ael/dictionary/uniform_dictionary.hpp>

using ael::dict::UniformDictionary;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(UniformDictionary, Construct) {
    [[maybe_unused]] const auto dict = UniformDictionary(1 << 16);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, Ord2Bytes) {
    const auto dict = UniformDictionary(1 << 16);
    EXPECT_EQ(dict.getWordOrd(2), 2);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, Ord3Bytes) {
    const auto dict = UniformDictionary(1 << 24);
    EXPECT_EQ(dict.getWordOrd(26), 26);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, OrdLongOrd) {
    const auto dict = UniformDictionary(1 << 16);
    EXPECT_EQ(dict.getWordOrd(256 * 8 + 42), 256 * 8 + 42);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, CumulativeNumFoundLow) {
    auto dict = UniformDictionary(256);
    const auto [low, _0, _1] = dict.getProbabilityStats(37);
    EXPECT_EQ(low, 37);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, CumulativeNumFoundLowZero) {
    auto dict = UniformDictionary(256);
    const auto [low, _0, _1] = dict.getProbabilityStats(0);
    EXPECT_EQ(low, 0);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, CumulativeNumFoundHigh) {
    auto dict = UniformDictionary(256);
    const auto [_0, high, _1] = dict.getProbabilityStats(37);
    EXPECT_EQ(high, 38);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, CumulativeNumFoundHighZero) {
    auto dict = UniformDictionary(256);
    const auto [_0, high, _1] = dict.getProbabilityStats(0);
    EXPECT_EQ(high, 1);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, TotalWordsCount) {
    auto dict = UniformDictionary(256);
    const auto [_0, _1, total] = dict.getProbabilityStats(0);
    EXPECT_EQ(total, 256);
}
