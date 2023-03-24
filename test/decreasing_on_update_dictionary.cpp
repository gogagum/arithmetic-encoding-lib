#include <gtest/gtest.h>

#include <array>
#include <cstdint>

#include <ael/dictionary/decreasing_on_update_dictionary.hpp>

using ael::dict::DecreasingOnUpdateDictionary;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, Construct) {
    const auto freqMapping = std::array{
        std::make_pair(std::uint64_t{42}, std::uint64_t(15)),
        std::make_pair(std::uint64_t{105}, std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary(256, freqMapping);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, TotalCount) {
    const auto freqMapping = std::array{
        std::make_pair(std::uint64_t{42}, std::uint64_t(15)),
        std::make_pair(std::uint64_t{105}, std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary(256, freqMapping);
    EXPECT_EQ(dict.getTotalWordsCnt(), 32);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, TotalCountEmpty) {
    const auto freqMapping = std::array<std::pair<std::uint64_t, std::uint64_t>, 0>{};
    auto dict = DecreasingOnUpdateDictionary(256, freqMapping);
    EXPECT_EQ(dict.getTotalWordsCnt(), 0);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, TotalCountAfterDecrease) {
    const auto freqMapping = std::array{
        std::make_pair(std::uint64_t{42}, std::uint64_t(15)),
        std::make_pair(std::uint64_t{105}, std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary(256 * 256, freqMapping);
    [[maybe_unused]] auto stats = dict.getProbabilityStats(42);

    EXPECT_EQ(dict.getTotalWordsCnt(), 31);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, CountAfterDecrease) {
    const auto freqMapping = std::array{
        std::make_pair(std::uint64_t{42}, std::uint64_t(15)),
        std::make_pair(std::uint64_t{105}, std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary(256 * 256, freqMapping);
    [[maybe_unused]] auto stats = dict.getProbabilityStats(42);

    auto [low, high, _0] = dict.getProbabilityStats(42);

    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 14);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, CountAfterDecreaseTwice) {
    const auto freqMapping = std::array{
        std::make_pair(std::uint64_t{42}, std::uint64_t(15)),
        std::make_pair(std::uint64_t{105}, std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary(256 * 256, freqMapping);
    [[maybe_unused]] auto stats = dict.getProbabilityStats(42);

    auto [low0, high0, _0] = dict.getProbabilityStats(42);

    EXPECT_EQ(low0, 0);
    EXPECT_EQ(high0, 14);

    auto [low1, high1, _1] = dict.getProbabilityStats(105);

    EXPECT_EQ(low1, 13);
    EXPECT_EQ(high1, 30);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, GetWord) {
    const auto freqMapping = std::array{
        std::make_pair(std::uint64_t{42}, std::uint64_t(15)),
        std::make_pair(std::uint64_t{105}, std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary(256 * 256, freqMapping);

    const auto ord = dict.getWordOrd(12);
    EXPECT_EQ(ord, 42);
}
