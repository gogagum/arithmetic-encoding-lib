#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

#include <ael/byte_data_constructor.hpp>

using ael::ByteDataConstructor;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(ByteDataConstructor, Consruct) {
    auto encoded = ByteDataConstructor();
}

//----------------------------------------------------------------------------//
TEST(ByteDataConstructor, PutBitFalse) {
    auto encoded = ByteDataConstructor();
    encoded.putBit(false);
    EXPECT_EQ(encoded.data<std::byte>()[0], std::byte{0b00000000});
}

//----------------------------------------------------------------------------//
TEST(ByteDataConstructor, PutBitTrue) {
    auto encoded = ByteDataConstructor();
    encoded.putBit(true);
    EXPECT_EQ(encoded.data<std::byte>()[0], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ByteDataConstructor, PutBits) {
    auto encoded = ByteDataConstructor();
    encoded.putBit(true);
    encoded.putBit(false);
    encoded.putBit(false);
    encoded.putBit(true);
    EXPECT_EQ(encoded.data<std::byte>()[0], std::byte{0b10010000});
}

//----------------------------------------------------------------------------//
TEST(ByteDataConstructor, PutBits2) {
    auto encoded = ByteDataConstructor();
    encoded.putBit(true);
    encoded.putBit(false);
    encoded.putBit(false);
    encoded.putBit(true);
    encoded.putBit(false);
    encoded.putBit(true);
    encoded.putBit(false);
    encoded.putBit(true);
    encoded.putBit(true);
    EXPECT_EQ(encoded.data<std::byte>()[0], std::byte{0b10010101});
    EXPECT_EQ(encoded.data<std::byte>()[1], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ByteDataConstructor, PutByte) {
    auto encoded = ByteDataConstructor();
    encoded.putByte(std::byte{42});
    EXPECT_EQ(encoded.data<std::byte>()[0], std::byte{42});
}

//----------------------------------------------------------------------------//
TEST(ByteDataConstructor, PutBitAfterByte) {
    auto encoded = ByteDataConstructor();
    encoded.putByte(std::byte{42});
    encoded.putBit(true);
    EXPECT_EQ(encoded.data<std::byte>()[0], std::byte{42});
    EXPECT_EQ(encoded.data<std::byte>()[1], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ByteDataConstructor, PutByte2) {
    auto encoded = ByteDataConstructor();
    encoded.putByte(std::byte{42});
    encoded.putByte(std::byte{37});
    encoded.putByte(std::byte{73});
    EXPECT_EQ(encoded.data<std::byte>()[0], std::byte{42});
    EXPECT_EQ(encoded.data<std::byte>()[1], std::byte{37});
    EXPECT_EQ(encoded.data<std::byte>()[2], std::byte{73});
}

//----------------------------------------------------------------------------//
TEST(ByteDataConstructor, PutInt64) {
    auto encoded = ByteDataConstructor();
    std::int64_t tested =
            0b0000000000100000100000000000100000000000000000100000000000100000;
            //0000000011111111222222223333333344444444555555556666666677777777

    encoded.putT(tested);
    EXPECT_EQ(encoded.size(), 8);
    EXPECT_EQ(encoded.data<std::byte>()[7], std::byte(0b00000000));
    EXPECT_EQ(encoded.data<std::byte>()[6], std::byte{0b00100000});
    EXPECT_EQ(encoded.data<std::byte>()[5], std::byte{0b10000000});
    EXPECT_EQ(encoded.data<std::byte>()[4], std::byte{0b00001000});
    EXPECT_EQ(encoded.data<std::byte>()[3], std::byte{0b00000000});
    EXPECT_EQ(encoded.data<std::byte>()[2], std::byte{0b00000010});
    EXPECT_EQ(encoded.data<std::byte>()[1], std::byte{0b00000000});
    EXPECT_EQ(encoded.data<std::byte>()[0], std::byte{0b00100000});
}

//----------------------------------------------------------------------------//
TEST(ByteDataConstructor, PutUInt32) {
    auto encoded = ByteDataConstructor();
    std::uint32_t tested = 0b00000000001000001000000000001000;
                           //00000000111111112222222233333333

    encoded.putT(tested);
    EXPECT_EQ(encoded.size(), 4);
    EXPECT_EQ(encoded.data<std::byte>()[3], std::byte(0b00000000));
    EXPECT_EQ(encoded.data<std::byte>()[2], std::byte{0b00100000});
    EXPECT_EQ(encoded.data<std::byte>()[1], std::byte{0b10000000});
    EXPECT_EQ(encoded.data<std::byte>()[0], std::byte{0b00001000});
}

