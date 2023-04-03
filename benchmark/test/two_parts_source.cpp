#include <gtest/gtest.h>

#include <two_parts_source.hpp>

// NOLINTBEGIN(cppcoreguidelines-owning-memory,
// cppcoreguidelines-avoid-magic-numbers)

TEST(TwoPartsSource, Construct) {
  auto src = TwoPartsSource::getGeneration(64, 6, 5.7, 34);
}

TEST(TwoPartsSource, InvalidEnthropy) {
  EXPECT_THROW(auto src = TwoPartsSource::getGeneration(64, 6, 6.1, 42),
               std::logic_error);
}

TEST(TwoPartsSource, ConstructGenerationInstance) {
  auto src = TwoPartsSource::getGeneration(64, 8, 5.5, 75);
}

TEST(TwoPartsSource, IterateGenerationInstance) {
  auto src = TwoPartsSource::getGeneration(64, 8, 5.5, 75);
  for (auto ord : src) {
  }
}

TEST(TwoPartsSource, IterateGenerationInstanceLength) {
  auto src = TwoPartsSource::getGeneration(64, 8, 5.5, 5, 75);
  std::vector<std::uint32_t> generated;
  std::copy(src.begin(), src.end(), std::back_inserter(generated));
  EXPECT_EQ(generated.size(), 5);
}

// NOLINTEND(cppcoreguidelines-owning-memory,
// cppcoreguidelines-avoid-magic-numbers)
