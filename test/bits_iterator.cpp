#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

#include <ael/bits_iterator.hpp>

using ael::impl::BitsIterator;
using ael::impl::ReverseBitsIterator;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsIterator, Construct) {
    const auto num = std::uint64_t{};
    [[maybe_unused]] const auto iter = BitsIterator(num, 3);
}

//----------------------------------------------------------------------------//
TEST(BitsIterator, Deref) {
    const auto num = std::uint64_t{0};
    auto iter = BitsIterator(num, 3);
    EXPECT_FALSE(*iter);
}

//----------------------------------------------------------------------------//
TEST(BitsIterator, Deref2) {
    const auto b = std::byte{0b11110000};
    auto iter = BitsIterator(b, 2);
    EXPECT_TRUE(*iter);
}

//----------------------------------------------------------------------------//
TEST(BitsIterator, Deref3) {
    const auto b = std::byte{0b11110000};
    auto iter = BitsIterator(b, 7);
    EXPECT_FALSE(*iter);
}

//----------------------------------------------------------------------------//
TEST(BitsIterator, DerefLongerT) {
    const auto num = std::uint16_t{0b1100110011110000};
    const auto iter1 = BitsIterator(num, 6);
    const auto iter2 = BitsIterator(num, 8);
    const auto iter3 = BitsIterator(num, 11);

    EXPECT_FALSE(*iter1);
    EXPECT_TRUE(*iter2);
    EXPECT_TRUE(*iter3);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(ReverseBitsIterator, Construct) {
    const auto num = std::uint32_t{};
    [[maybe_unused]] const auto iter = ReverseBitsIterator(num, 3);
}

//----------------------------------------------------------------------------//
TEST(ReverseBitsIterator, Deref) {
    const auto num = std::byte{0b11110000};
    const auto iter = ReverseBitsIterator(num, 2);
    EXPECT_FALSE(*iter);
}

//----------------------------------------------------------------------------//
TEST(ReverseBitsIterator, Deref2) {
    const auto num = std::byte{0b11110000};
    const auto iter = ReverseBitsIterator(num, 3);
    EXPECT_FALSE(*iter);
}

//----------------------------------------------------------------------------//
TEST(ReverseBitsIterator, Deref3) {
    const auto num = std::byte{0b11110000};
    const auto iter = ReverseBitsIterator(num, 7);
    EXPECT_TRUE(*iter);
}

//----------------------------------------------------------------------------//
TEST(ReverseBitsIterator, DerefLongerT) {
    const auto num = std::uint16_t{0b1100110011110000};
    const auto iter1 = ReverseBitsIterator(num, 6);
    const auto iter2 = ReverseBitsIterator(num, 8);
    const auto iter3 = ReverseBitsIterator(num, 11);

    EXPECT_TRUE(*iter1);
    EXPECT_FALSE(*iter2);
    EXPECT_TRUE(*iter3);
}
