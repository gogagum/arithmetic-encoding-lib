#include <gtest/gtest.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/arithmetic_decoder.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/dictionary/adaptive_dictionary.hpp>
#include <boost/range/combine.hpp>
#include <random>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

TEST(AdaptiveEncodeDecode, EncodeEmpty) {
  const auto encoded = std::vector<std::uint64_t>();
  auto dict = ael::dict::AdaptiveDictionary({6, 10});
  auto dataConstructor = ael::ByteDataConstructor();
  auto [wordsCount, bitsCount] =
      ael::ArithmeticCoder::encode(encoded, dataConstructor, dict);

  EXPECT_EQ(wordsCount, 0);
  EXPECT_EQ(bitsCount, 2);
  EXPECT_EQ(dataConstructor.size(), 1);
}

TEST(AdaptiveEncodeDecode, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = ael::dict::AdaptiveDictionary({6, 7});
  auto dataParser = ael::DataParser(data);
  auto retOrds = std::vector<std::uint32_t>();
  ael::ArithmeticDecoder::decode(dataParser, dict, std::back_inserter(retOrds),
                                 {0, 0});

  EXPECT_EQ(retOrds.size(), 0);
}

TEST(AdaptiveEncodeDecode, EncodeDecodeEmptySequence) {
  const auto encoded = std::vector<std::uint64_t>();
  auto dataConstructor = ael::ByteDataConstructor();
  auto decoded = std::vector<std::uint64_t>();

  {
    auto dict = ael::dict::AdaptiveDictionary({6, 7});
    auto [wordsCount, bitsCount] =
        ael::ArithmeticCoder::encode(encoded, dataConstructor, dict);
  }

  {
    auto dict = ael::dict::AdaptiveDictionary({6, 7});
    auto dataParser = ael::DataParser(
        std::span(dataConstructor.data<std::byte>(), dataConstructor.size()));
    ael::ArithmeticDecoder::decode(
        dataParser, dict, std::back_inserter(decoded),
        {encoded.size(), std::numeric_limits<std::size_t>::max()});
  }

  EXPECT_EQ(encoded.size(), decoded.size());
}

TEST(AdaptiveEncodeDecode, EncodeSmall) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 4, 5, 2};
  auto dict = ael::dict::AdaptiveDictionary({6, 7});
  auto dataConstructor = ael::ByteDataConstructor();
  auto [wordsCount, bitsCount] =
      ael::ArithmeticCoder::encode(encoded, dataConstructor, dict);

  EXPECT_EQ(wordsCount, 5);
  EXPECT_GE(dataConstructor.size(), 0);
}

TEST(AdaptiveEncodeDecode, EncodeDecodeSmallSequence) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 5, 7, 2};
  auto dataConstructor = ael::ByteDataConstructor();
  auto decoded = std::vector<std::uint64_t>();

  {
    auto dict = ael::dict::AdaptiveDictionary({8, 5});
    const auto [wordsCount, bitsCount] =
        ael::ArithmeticCoder::encode(encoded, dataConstructor, dict);
  }

  {
    auto dict = ael::dict::AdaptiveDictionary({8, 5});
    auto dataParser = ael::DataParser(
        std::span(dataConstructor.data<std::byte>(), dataConstructor.size()));
    ael::ArithmeticDecoder::decode(
        dataParser, dict, std::back_inserter(decoded),
        {encoded.size(), std::numeric_limits<std::size_t>::max()});
  }

  EXPECT_EQ(encoded.size(), decoded.size());

  for (auto [enc, dec] : boost::range::combine(encoded, decoded)) {
    EXPECT_EQ(enc, dec);
  }
}

TEST(AdaptiveEncodeDecode, EncodeDecodeSmallSequenceBitsLimit) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 5, 7, 2};
  auto dataConstructor = ael::ByteDataConstructor();
  auto decoded = std::vector<std::uint64_t>();

  auto dict0 = ael::dict::AdaptiveDictionary({8, 5});
  const auto [wordsCount, bitsCount] =
      ael::ArithmeticCoder::encode(encoded, dataConstructor, dict0);

  {
    auto dict1 = ael::dict::AdaptiveDictionary({8, 5});
    auto dataParser = ael::DataParser(
        std::span(dataConstructor.data<std::byte>(), dataConstructor.size()));
    ael::ArithmeticDecoder::decode(dataParser, dict1,
                                   std::back_inserter(decoded),
                                   {wordsCount, bitsCount});
  }

  EXPECT_EQ(encoded.size(), decoded.size());

  for (auto [enc, dec] : boost::range::combine(encoded, decoded)) {
    EXPECT_EQ(enc, dec);
  }
}

TEST(AdaptiveEncodeDecode, EncodeDecodeFuzz) {
  auto gen = std::mt19937(42);

  for (auto iteration : std::ranges::iota_view(0, 50)) {
    const std::size_t length = gen() % 250;
    const std::uint32_t rng = gen() % 256;

    auto encoded = std::vector<std::uint32_t>{};

    for (const auto _ : std::ranges::iota_view(std::size_t{0}, length)) {
      encoded.push_back(gen() % rng);
    }

    auto dataConstructor = ael::ByteDataConstructor();
    auto decoded = std::vector<std::uint64_t>();

    const std::size_t ratio = gen() % 10 + 5;  // [5..15)

    {
      auto dict = ael::dict::AdaptiveDictionary({rng, ratio});
      const auto [wordsCount, bitsCount] =
          ael::ArithmeticCoder::encode(encoded, dataConstructor, dict);
    }

    {
      auto dict = ael::dict::AdaptiveDictionary({rng, ratio});
      auto dataParser = ael::DataParser(
          std::span(dataConstructor.data<std::byte>(), dataConstructor.size()));
      ael::ArithmeticDecoder::decode(
          dataParser, dict, std::back_inserter(decoded),
          {encoded.size(), std::numeric_limits<std::size_t>::max()});
    }

    EXPECT_EQ(encoded.size(), decoded.size());

    for (auto [enc, dec] : boost::range::combine(encoded, decoded)) {
      EXPECT_EQ(enc, dec);
    }
  }
}

TEST(AdaptiveEncodeDecode, EncodeDecodeFuzzBitsLimit) {
  auto gen = std::mt19937(42);

  for (auto iteration : std::ranges::iota_view(0, 50)) {
    const std::size_t length = gen() % 250;
    const std::uint32_t rng = gen() % 256;

    auto encoded = std::vector<std::uint32_t>{};

    for (const auto _ : std::ranges::iota_view(std::size_t{0}, length)) {
      encoded.push_back(gen() % rng);
    }

    auto dataConstructor = ael::ByteDataConstructor();
    auto decoded = std::vector<std::uint64_t>();

    const std::size_t ratio = gen() % 10 + 5;  // [5..15)

    auto dict1 = ael::dict::AdaptiveDictionary({rng, ratio});
    const auto [wordsCount, bitsCount] =
        ael::ArithmeticCoder::encode(encoded, dataConstructor, dict1);

    {
      auto dict2 = ael::dict::AdaptiveDictionary({rng, ratio});
      auto dataParser = ael::DataParser(
          std::span(dataConstructor.data<std::byte>(), dataConstructor.size()));
      ael::ArithmeticDecoder::decode(dataParser, dict2,
                                     std::back_inserter(decoded),
                                     {wordsCount, bitsCount});
    }

    EXPECT_EQ(encoded.size(), decoded.size());

    for (auto [enc, dec] : boost::range::combine(encoded, decoded)) {
      EXPECT_EQ(enc, dec);
    }
  }
}

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
