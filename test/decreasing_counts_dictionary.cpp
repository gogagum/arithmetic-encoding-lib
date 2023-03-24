#include <gtest/gtest.h>

#include <ael/dictionary/decreasing_counts_dictionary.hpp>

using ael::dict::DecreasingCountDictionary;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(DecreasingCountDictionary, Construct) {
    [[maybe_unused]] auto dict = DecreasingCountDictionary(42);
}

//----------------------------------------------------------------------------//
TEST(DecreasingCountDictionary, TotalCount) {
    auto dict = DecreasingCountDictionary(42);
    EXPECT_EQ(dict.getTotalWordsCnt(), 42);
}

//----------------------------------------------------------------------------//
TEST(DecreasingCountDictionary, TotalAfterUpdate) {
    auto dict = DecreasingCountDictionary<std::uint32_t>(42);
    [[maybe_unused]] auto _ = dict.getProbabilityStats(14);
    EXPECT_EQ(dict.getTotalWordsCnt(), 14);
}

//----------------------------------------------------------------------------//
TEST(DecreasingCountDictionary, StatsAfterUpdate) {
    auto dict = DecreasingCountDictionary<std::uint32_t>(42);
    auto [low, high, total] = dict.getProbabilityStats(14);
    EXPECT_EQ(low, 13);
    EXPECT_EQ(high, 14);
    EXPECT_EQ(total, 42);
}
