// Uncomment these three lines to edit tests using IDE:

// #define TESTED_CLASS ael::dict::AdaptiveADictionary
// #define TEST_SUIT_NAME AdaptiveAEncodeDecode
// #include <ael/dictionary/adaptive_a_dictionary.hpp>

#if defined(TEST_SUIT_NAME) && defined(TESTED_CLASS)

#include <gtest/gtest.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/arithmetic_decoder.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ord_generator.hpp>
#include <random>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

TEST(TEST_SUIT_NAME, EncodeEmpty) {
  const auto encoded = std::vector<std::uint64_t>();
  auto dict = TESTED_CLASS(6);
  auto [dataConstructor, wordsCount, bitsCount] =
      ael::ArithmeticCoder().encode(encoded, dict).finalize();

  EXPECT_EQ(wordsCount, 0);
  EXPECT_EQ(bitsCount, 2);
  EXPECT_EQ(dataConstructor->size(), 1);
}

TEST(TEST_SUIT_NAME, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = TESTED_CLASS(6);
  auto dataParser = ael::DataParser(data);
  auto retOrds = std::vector<std::uint32_t>();
  ael::ArithmeticDecoder::decode(dataParser, dict, std::back_inserter(retOrds),
                                 {0, 0});

  EXPECT_EQ(retOrds.size(), 0);
}

TEST(TEST_SUIT_NAME, EncodeDecodeEmptySequence) {
  const auto encoded = std::vector<std::uint64_t>();
  auto decoded = std::vector<std::uint64_t>();

  auto dict = TESTED_CLASS(6);
  auto [dataConstructor, wordsCount, bitsCount] =
      ael::ArithmeticCoder().encode(encoded, dict).finalize();

  {
    auto dict = TESTED_CLASS(6);
    auto dataParser = ael::DataParser(
        std::span(dataConstructor->data<std::byte>(), dataConstructor->size()));
    ael::ArithmeticDecoder::decode(
        dataParser, dict, std::back_inserter(decoded),
        {encoded.size(), std::numeric_limits<std::size_t>::max()});
  }

  EXPECT_EQ(encoded.size(), decoded.size());
}

TEST(TEST_SUIT_NAME, EncodeSmall) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 4, 5, 2};
  auto dict = TESTED_CLASS(6);
  auto [dataConstructor, wordsCount, bitsCount] =
      ael::ArithmeticCoder().encode(encoded, dict).finalize();

  EXPECT_EQ(wordsCount, 5);
  EXPECT_GE(dataConstructor->size(), 0);
}

TEST(TEST_SUIT_NAME, EncodeDecodeSmallSequence) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 5, 7, 2};
  auto decoded = std::vector<std::uint64_t>();

  auto dict = TESTED_CLASS(8);
  auto [dataConstructor, wordsCount, bitsCount] =
      ael::ArithmeticCoder().encode(encoded, dict).finalize();

  {
    auto dict = TESTED_CLASS(8);
    auto dataParser = ael::DataParser(
        std::span(dataConstructor->data<std::byte>(), dataConstructor->size()));
    ael::ArithmeticDecoder::decode(
        dataParser, dict, std::back_inserter(decoded),
        {encoded.size(), std::numeric_limits<std::size_t>::max()});
  }

  EXPECT_EQ(encoded.size(), decoded.size());
  EXPECT_EQ(0, std::memcmp(encoded.data(), decoded.data(),
                           encoded.size() * sizeof(std::uint64_t)));
}

TEST(TEST_SUIT_NAME, EncodeDecodeSmallSequenceBitsLimit) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 5, 7, 2};
  auto decoded = std::vector<std::uint64_t>();

  auto dict0 = TESTED_CLASS(8);
  auto [dataConstructor, wordsCount, bitsCount] =
      ael::ArithmeticCoder().encode(encoded, dict0).finalize();

  {
    auto dict1 = TESTED_CLASS(8);
    auto dataParser = ael::DataParser(
        std::span(dataConstructor->data<std::byte>(), dataConstructor->size()));
    ael::ArithmeticDecoder::decode(dataParser, dict1,
                                   std::back_inserter(decoded),
                                   {wordsCount, bitsCount});
  }

  EXPECT_EQ(encoded.size(), decoded.size());
  EXPECT_EQ(0, std::memcmp(encoded.data(), decoded.data(),
                           encoded.size() * sizeof(std::uint64_t)));
}

TEST(TEST_SUIT_NAME, EncodeDecodeFuzz) {
  auto gen = std::mt19937(42);

  for (auto iteration : std::ranges::iota_view(0, 15)) {
    const std::size_t length = gen() % 250;
    const std::uint32_t rng = gen() % 256;

    auto encoded = boost::container::static_vector<std::uint64_t, 250>{};
    std::generate_n(std::back_inserter(encoded), length,
                    ael::test::OrdGenerator(gen, rng));

    auto decoded = std::vector<std::uint64_t>();

    auto dict = TESTED_CLASS(rng);
    const auto [dataConstructor, wordsCount, bitsCount] =
        ael::ArithmeticCoder().encode(encoded, dict).finalize();

    {
      auto dict = TESTED_CLASS(rng);
      auto dataParser = ael::DataParser(
          std::span(dataConstructor->data<std::byte>(), dataConstructor->size()));
      ael::ArithmeticDecoder::decode(
          dataParser, dict, std::back_inserter(decoded),
          {encoded.size(), std::numeric_limits<std::size_t>::max()});
    }

    EXPECT_EQ(encoded.size(), decoded.size());
    EXPECT_EQ(0, std::memcmp(encoded.data(), decoded.data(),
                             encoded.size() * sizeof(std::uint64_t)));
  }
}

TEST(TEST_SUIT_NAME, EncodeDecodeFuzzBitsLimit) {
  auto gen = std::mt19937(42);

  for (auto iteration : std::ranges::iota_view(0, 15)) {
    const std::size_t length = gen() % 250;
    const std::uint32_t rng = gen() % 256;

    auto encoded = boost::container::static_vector<std::uint64_t, 250>{};
    std::generate_n(std::back_inserter(encoded), length,
                    ael::test::OrdGenerator(gen, rng));

    auto decoded = std::vector<std::uint64_t>();

    auto dict1 = TESTED_CLASS(rng);
    const auto [dataConstructor, wordsCount, bitsCount] =
        ael::ArithmeticCoder().encode(encoded, dict1).finalize();

    {
      auto dict2 = TESTED_CLASS(rng);
      auto dataParser = ael::DataParser(
          std::span(dataConstructor->data<std::byte>(), dataConstructor->size()));
      ael::ArithmeticDecoder::decode(dataParser, dict2,
                                     std::back_inserter(decoded),
                                     {wordsCount, bitsCount});
    }

    EXPECT_EQ(encoded.size(), decoded.size());
    EXPECT_EQ(0, std::memcmp(encoded.data(), decoded.data(),
                             encoded.size() * sizeof(std::uint64_t)));
  }
}

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

#endif
