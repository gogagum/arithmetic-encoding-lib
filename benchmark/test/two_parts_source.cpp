#include <gtest/gtest.h>

#include <two_parts_source.hpp>

// NOLINTBEGIN(cppcoreguidelines-owning-memory,
// cppcoreguidelines-avoid-magic-numbers)

TEST(TwoPartsSource, Construct) {
  auto src = TwoPartsSource<std::uint32_t, 56>(6, 5.7);
}

TEST(TwoPartsSource, InvalitEnthropy) {
  EXPECT_THROW(auto src = (TwoPartsSource<std::uint32_t, 56>(6, 6)),
               std::logic_error);
}

// NOLINTEND(cppcoreguidelines-owning-memory,
// cppcoreguidelines-avoid-magic-numbers)
