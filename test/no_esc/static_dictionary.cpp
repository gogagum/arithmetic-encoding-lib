#include <gtest/gtest.h>

#include <ael/dictionary/static_dictionary.hpp>
#include <array>
#include <cstdint>

using ael::dict::StaticDictionary;

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

TEST(StaticDictionary, ConstructFromCumulativeNumFound) {
  const auto cumulFreq = std::array<StaticDictionary::CountMapping, 0>{};
  const auto dict = StaticDictionary(256, cumulFreq);
}

TEST(StaticDictionary, Ord2Bytes1) {
  const auto countMap = std::array{StaticDictionary::CountMapping{42, 37},
                                   StaticDictionary::CountMapping{112, 5}};

  const auto dict = StaticDictionary(256ul * 256u, countMap);

  EXPECT_EQ(dict.getWordOrd(37), 112);
}

TEST(StaticDictionary, Ord2BytesFromFreq) {
  const auto countMap = std::array{
      StaticDictionary::CountMapping{0, std::uint64_t{2}},  // 2
      StaticDictionary::CountMapping{1, std::uint64_t{3}},  // 5
      StaticDictionary::CountMapping{2, std::uint64_t{1}},  // 6
      StaticDictionary::CountMapping{3, std::uint64_t{2}},  // 8
      StaticDictionary::CountMapping{4, std::uint64_t{5}},  // 13
      StaticDictionary::CountMapping{5, std::uint64_t{9}},  // 22
      StaticDictionary::CountMapping{6, std::uint64_t{3}},  // 25
      StaticDictionary::CountMapping{7, std::uint64_t{0}}   // 25
  };

  const auto dict = StaticDictionary((1 << 24), countMap);

  EXPECT_EQ(dict.getWordOrd(5), 2);
  EXPECT_EQ(dict.getWordOrd(15), 5);
}

TEST(StaticDictionary, Ord2Bytes2) {
  const auto countMap =
      std::array{StaticDictionary::CountMapping{42, std::uint64_t{37}},
                 StaticDictionary::CountMapping{112, std::uint64_t{42}}};

  const auto dict = StaticDictionary((1 << 24), countMap);

  EXPECT_EQ(dict.getWordOrd(36), 42);
}

TEST(StaticDictionary, CumulativeNumFoundLow) {
  auto countMap =
      std::array{StaticDictionary::CountMapping{42, std::uint64_t{37}},
                 StaticDictionary::CountMapping{112, std::uint64_t{5}}};

  auto dict = StaticDictionary((1 << 16), countMap);

  const auto [low1, _0, _1] = dict.getProbabilityStats(112);
  const auto [_2, high2, _3] = dict.getProbabilityStats(113);

  EXPECT_EQ(low1, 37);
  EXPECT_EQ(high2, 42);
}

TEST(StaticDictionary, CumulativeNumFoundZero) {
  const auto countMap =
      std::array{StaticDictionary::CountMapping{42, std::uint64_t{37}},
                 StaticDictionary::CountMapping{112, std::uint64_t{42}}};

  auto dict = StaticDictionary(256, countMap);

  const auto [low, _0, _1] = dict.getProbabilityStats(1);

  EXPECT_EQ(low, 0);
}

TEST(StaticDictionary, CumulativeNumFoundHigh) {
  auto countMap =
      std::array{StaticDictionary::CountMapping{42, std::uint64_t{37}},
                 StaticDictionary::CountMapping{112, std::uint64_t{5}}};

  auto dict = StaticDictionary((1 << 16), countMap);

  const auto [_0, high1, _1] = dict.getProbabilityStats(111);
  const auto [_2, high2, _3] = dict.getProbabilityStats(112);

  EXPECT_EQ(high1, 37);
  EXPECT_EQ(high2, 42);
}

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
