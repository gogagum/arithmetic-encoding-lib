#include <gtest/gtest.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/arithmetic_decoder.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/dictionary/adaptive_dictionary.hpp>
#include <ord_generator.hpp>
#include <random>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

TEST(AdaptiveEncodeDecode, EncodeEmpty) {
  const auto encoded = std::vector<std::uint64_t>();
  auto dict = ael::dict::AdaptiveDictionary({6, 10});
  auto [dataConstructor, wordsCount, bitsCount] =
      ael::ArithmeticCoder().encode(encoded, dict).finalize();

  EXPECT_EQ(wordsCount, 0);
  EXPECT_EQ(bitsCount, 2);
  EXPECT_EQ(dataConstructor->size(), 1);
}

TEST(AdaptiveEncodeDecode, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = ael::dict::AdaptiveDictionary({6, 7});
  auto dataParser = ael::DataParser(data);
  auto retOrds = std::vector<std::uint32_t>();
  ael::ArithmeticDecoder(dataParser, 0).decode(dict,
                                   std::back_inserter(retOrds), 0);

  EXPECT_EQ(retOrds.size(), 0);
}

TEST(AdaptiveEncodeDecode, EncodeDecodeEmptySequence) {
  const auto encoded = std::vector<std::uint64_t>();
  auto decoded = std::vector<std::uint64_t>();

  auto dict = ael::dict::AdaptiveDictionary({6, 7});
  auto [dataConstructor, wordsCount, bitsCount] =
      ael::ArithmeticCoder().encode(encoded, dict).finalize();

  {
    auto dict = ael::dict::AdaptiveDictionary({6, 7});
    auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
    ael::ArithmeticDecoder(dataParser).decode(
        dict, std::back_inserter(decoded), encoded.size());
  }

  EXPECT_EQ(encoded.size(), decoded.size());
}

TEST(AdaptiveEncodeDecode, EncodeSmall) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 4, 5, 2};
  auto dict = ael::dict::AdaptiveDictionary({6, 7});
  auto [dataConstructor, wordsCount, bitsCount] =
      ael::ArithmeticCoder().encode(encoded, dict).finalize();

  EXPECT_EQ(wordsCount, 5);
  EXPECT_GE(dataConstructor->size(), 0);
}

TEST(AdaptiveEncodeDecode, EncodeDecodeSmallSequence) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 5, 7, 2};
  auto decoded = std::vector<std::uint64_t>();

  auto dict = ael::dict::AdaptiveDictionary({8, 5});
  const auto [dataConstructor, wordsCount, bitsCount] =
      ael::ArithmeticCoder().encode(encoded, dict).finalize();

  {
    auto dict = ael::dict::AdaptiveDictionary({8, 5});
    auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
    ael::ArithmeticDecoder(dataParser).decode(
        dict, std::back_inserter(decoded), encoded.size());
  }

  EXPECT_EQ(encoded.size(), decoded.size());
  EXPECT_EQ(0, std::memcmp(encoded.data(), decoded.data(),
                           encoded.size() * sizeof(std::uint64_t)));
}

TEST(AdaptiveEncodeDecode, EncodeDecodeSmallSequenceBitsLimit) {
  const auto encoded = std::vector<std::uint64_t>{5, 3, 5, 7, 2};
  auto decoded = std::vector<std::uint64_t>();

  auto dict0 = ael::dict::AdaptiveDictionary({8, 5});
  const auto [dataConstructor, wordsCount, bitsCount] =
      ael::ArithmeticCoder().encode(encoded, dict0).finalize();

  {
    auto dict1 = ael::dict::AdaptiveDictionary({8, 5});
    auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
    ael::ArithmeticDecoder(dataParser, bitsCount).decode(
        dict1, std::back_inserter(decoded), wordsCount);
  }

  EXPECT_EQ(encoded.size(), decoded.size());
  EXPECT_EQ(0, std::memcmp(encoded.data(), decoded.data(),
                           encoded.size() * sizeof(std::uint64_t)));
}

TEST(AdaptiveEncodeDecode, EncodeDecodeFuzz) {
  auto gen = std::mt19937(42);

  for (auto iteration : std::ranges::iota_view(0, 50)) {
    const std::size_t length = gen() % 250;
    const std::uint32_t rng = gen() % 256;

    auto encoded = boost::container::static_vector<std::uint64_t, 250>{};
    std::generate_n(std::back_inserter(encoded), length,
                    ael::test::OrdGenerator(gen, rng));

    auto decoded = std::vector<std::uint64_t>();

    const std::size_t ratio = gen() % 10 + 5;  // [5..15)

    auto dict = ael::dict::AdaptiveDictionary({rng, ratio});
    const auto [dataConstructor, wordsCount, bitsCount] =
        ael::ArithmeticCoder().encode(encoded, dict).finalize();

    {
      auto dict = ael::dict::AdaptiveDictionary({rng, ratio});
      auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
      ael::ArithmeticDecoder(dataParser).decode(
          dict, std::back_inserter(decoded), encoded.size());
    }

    EXPECT_EQ(encoded.size(), decoded.size());
    EXPECT_EQ(0, std::memcmp(encoded.data(), decoded.data(),
                             encoded.size() * sizeof(std::uint64_t)));
  }
}

TEST(AdaptiveEncodeDecode, EncodeDecodeFuzzBitsLimit) {
  auto gen = std::mt19937(42);

  for (auto iteration : std::ranges::iota_view(0, 50)) {
    const std::size_t length = gen() % 250;
    const std::uint32_t rng = gen() % 256;

    auto encoded = boost::container::static_vector<std::uint64_t, 250>{};
    std::generate_n(std::back_inserter(encoded), length,
                    ael::test::OrdGenerator(gen, rng));

    auto decoded = std::vector<std::uint64_t>();

    const std::size_t ratio = gen() % 10 + 5;  // [5..15)

    auto dict1 = ael::dict::AdaptiveDictionary({rng, ratio});
    const auto [dataConstructor, wordsCount, bitsCount] =
        ael::ArithmeticCoder().encode(encoded, dict1).finalize();

    {
      auto dict2 = ael::dict::AdaptiveDictionary({rng, ratio});
      auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
      ael::ArithmeticDecoder(dataParser, bitsCount).decode(
          dict2, std::back_inserter(decoded), wordsCount);
    }

    EXPECT_EQ(encoded.size(), decoded.size());
    EXPECT_EQ(0, std::memcmp(encoded.data(), decoded.data(),
                             encoded.size() * sizeof(std::uint64_t)));
  }
}

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
