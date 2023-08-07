#include <gtest/gtest.h>

#include <ael/esc/arithmetic_coder.hpp>
#include <ael/esc/arithmetic_decoder.hpp>
#include <ael/esc/dictionary/ppmd_dictionary.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <boost/range/combine.hpp>
#include <random>

#define TEST_SUIT_NAME EscPPMDDictionary
#define TESTED_CLASS ael::esc::dict::PPMDDictionary

// NOLINTBEGIN(cppcoreguidelines-*)

TEST(TEST_SUIT_NAME, EncodeEmpty) {
  const auto encoded = std::vector<std::uint64_t>();
  auto dict = TESTED_CLASS({6, 2});
  auto dataConstructor = ael::ByteDataConstructor();
  auto [wordsCount, bitsCount] =
      ael::esc::ArithmeticCoder::encode(encoded, dataConstructor, dict);

  EXPECT_EQ(wordsCount, 0);
  EXPECT_EQ(bitsCount, 2);
  EXPECT_EQ(dataConstructor.size(), 1);
}

TEST(TEST_SUIT_NAME, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = TESTED_CLASS({6, 3});
  auto dataParser = ael::DataParser(data);
  auto retOrds = std::vector<std::uint32_t>();
  ael::esc::ArithmeticDecoder::decode(dataParser, dict, std::back_inserter(retOrds),
                                 {0, 0});

  EXPECT_EQ(retOrds.size(), 0);
}

TEST(TEST_SUIT_NAME, EncodeDecodeEmptySequence) {
  const auto encoded = std::vector<std::uint64_t>();
  auto dataConstructor = ael::ByteDataConstructor();
  auto decoded = std::vector<std::uint64_t>();

  {
    auto dict = TESTED_CLASS({6, 3});
    auto [wordsCount, bitsCount] =
        ael::esc::ArithmeticCoder::encode(encoded, dataConstructor, dict);
  }

  {
    auto dict = TESTED_CLASS({6, 3});
    auto dataParser = ael::DataParser(
        std::span(dataConstructor.data<std::byte>(), dataConstructor.size()));
    ael::esc::ArithmeticDecoder::decode(
        dataParser, dict, std::back_inserter(decoded),
        {encoded.size(), std::numeric_limits<std::size_t>::max()});
  }

  EXPECT_EQ(encoded.size(), decoded.size());
}

TEST(TEST_SUIT_NAME, EncodeSmall) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 4, 5, 2};
  auto dict = TESTED_CLASS({6, 3});
  auto dataConstructor = ael::ByteDataConstructor();
  auto [wordsCount, bitsCount] =
      ael::esc::ArithmeticCoder::encode(encoded, dataConstructor, dict);

  EXPECT_EQ(wordsCount, 10);
  EXPECT_GE(dataConstructor.size(), 0);
}

TEST(TEST_SUIT_NAME, EncodeDecodeSmallSequence) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 5, 7, 2};
  auto dataConstructor = ael::ByteDataConstructor();
  auto decoded = std::vector<std::uint64_t>();

  auto dict = TESTED_CLASS({8, 5});
  const auto [wordsCount, bitsCount] =
      ael::esc::ArithmeticCoder::encode(encoded, dataConstructor, dict);

  {
    auto dict = TESTED_CLASS({8, 5});
    auto dataParser = ael::DataParser(
        std::span(dataConstructor.data<std::byte>(), dataConstructor.size()));
    ael::esc::ArithmeticDecoder::decode(
        dataParser, dict, std::back_inserter(decoded),
        {wordsCount, std::numeric_limits<std::size_t>::max()});
  }

  EXPECT_EQ(encoded.size(), decoded.size());

  for (auto [enc, dec] : boost::range::combine(encoded, decoded)) {
    EXPECT_EQ(enc, dec);
  }
}

TEST(TEST_SUIT_NAME, EncodeDecodeSmallSequenceBitsLimit) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 5, 7, 2};
  auto dataConstructor = ael::ByteDataConstructor();
  auto decoded = std::vector<std::uint64_t>();

  auto dict0 = TESTED_CLASS({8, 5});
  const auto [wordsCount, bitsCount] =
      ael::esc::ArithmeticCoder::encode(encoded, dataConstructor, dict0);

  {
    auto dict1 = TESTED_CLASS({8, 5});
    auto dataParser = ael::DataParser(
        std::span(dataConstructor.data<std::byte>(), dataConstructor.size()));
    ael::esc::ArithmeticDecoder::decode(dataParser, dict1,
                                   std::back_inserter(decoded),
                                   {wordsCount, bitsCount});
  }

  EXPECT_EQ(encoded.size(), decoded.size());

  for (auto [enc, dec] : boost::range::combine(encoded, decoded)) {
    EXPECT_EQ(enc, dec);
  }
}

TEST(TEST_SUIT_NAME, EncodeDecodeFuzz) {
  auto gen = std::mt19937(42);

  for (auto iteration : std::ranges::iota_view(0, 15)) {
    const std::size_t length = gen() % 250;
    const std::uint32_t rng = gen() % 256;

    auto encoded = std::vector<std::uint32_t>{};

    for (const auto _ : std::ranges::iota_view(std::size_t{0}, length)) {
      encoded.push_back(gen() % rng);
    }

    auto dataConstructor = ael::ByteDataConstructor();
    auto decoded = std::vector<std::uint64_t>();

    auto dict = TESTED_CLASS({rng, 5});
    const auto [wordsCount, bitsCount] =
        ael::esc::ArithmeticCoder::encode(encoded, dataConstructor, dict);

    {
      auto dict = TESTED_CLASS({rng, 5});
      auto dataParser = ael::DataParser(
          std::span(dataConstructor.data<std::byte>(), dataConstructor.size()));
      ael::esc::ArithmeticDecoder::decode(
          dataParser, dict, std::back_inserter(decoded),
          {wordsCount, std::numeric_limits<std::size_t>::max()});
    }

    assert(encoded.size() == decoded.size());
    EXPECT_EQ(encoded.size(), decoded.size());

    for (auto [enc, dec] : boost::range::combine(encoded, decoded)) {
      assert(enc == dec);
      EXPECT_EQ(enc, dec);
    }
  }
}

TEST(TEST_SUIT_NAME, EncodeDecodeFuzzBitsLimit) {
  auto gen = std::mt19937(42);

  for (auto iteration : std::ranges::iota_view(0, 15)) {
    const std::size_t length = gen() % 250;
    const std::uint32_t rng = gen() % 256;

    auto encoded = std::vector<std::uint32_t>{};

    for (const auto _ : std::ranges::iota_view(std::size_t{0}, length)) {
      encoded.push_back(gen() % rng);
    }

    auto dataConstructor = ael::ByteDataConstructor();
    auto decoded = std::vector<std::uint64_t>();

    auto dict1 = TESTED_CLASS({rng, 5});
    const auto [wordsCount, bitsCount] =
        ael::esc::ArithmeticCoder::encode(encoded, dataConstructor, dict1);

    {
      auto dict2 = TESTED_CLASS({rng, 5});
      auto dataParser = ael::DataParser(
          std::span(dataConstructor.data<std::byte>(), dataConstructor.size()));
      ael::esc::ArithmeticDecoder::decode(dataParser, dict2,
                                     std::back_inserter(decoded),
                                     {wordsCount, bitsCount});
    }

    EXPECT_EQ(encoded.size(), decoded.size());

    for (auto [enc, dec] : boost::range::combine(encoded, decoded)) {
      EXPECT_EQ(enc, dec);
    }
  }
}

// NOLINTEND(cppcoreguidelines-*)