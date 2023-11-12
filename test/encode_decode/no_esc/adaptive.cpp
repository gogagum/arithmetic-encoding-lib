#include <gtest/gtest.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/arithmetic_decoder.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/dictionary/adaptive_dictionary.hpp>
#include <encode_decode_test.hpp>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

namespace {

namespace rng = std::ranges;

using ael::ArithmeticCoder;
using ael::ArithmeticDecoder;

using AdaptiveEncodeDecode = EncodeDecodeTest<ael::dict::AdaptiveDictionary>;

TEST_F(AdaptiveEncodeDecode, EncodeEmpty) {
  auto dict = ael::dict::AdaptiveDictionary({6, 10});
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(encoded, dict).finalize();

  EXPECT_EQ(wordsCnt, 0);
  EXPECT_EQ(bitsCnt, 2);
  EXPECT_EQ(dataConstructor->size(), 1);
}

TEST_F(AdaptiveEncodeDecode, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = ael::dict::AdaptiveDictionary({6, 7});
  auto dataParser = ael::DataParser(data);
  ArithmeticDecoder(dataParser, 0).decode(dict, outIter, 0);

  EXPECT_EQ(decoded.size(), 0);
}

TEST_F(AdaptiveEncodeDecode, EncodeDecodeEmptySequence) {
  auto dict0 = ael::dict::AdaptiveDictionary({8, 7});
  auto [dataConstructor, wordsCnt, _] =
      ArithmeticCoder().encode(encoded, dict0).finalize();

  auto dict1 = ael::dict::AdaptiveDictionary({8, 7});
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict1, outIter, wordsCnt);

  EXPECT_EQ(encoded.size(), decoded.size());
}

TEST_F(AdaptiveEncodeDecode, EncodeSmall) {
  auto dict = ael::dict::AdaptiveDictionary({8, 7});
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(smallSequence, dict).finalize();

  EXPECT_EQ(wordsCnt, 5);
  EXPECT_GE(dataConstructor->size(), 0);
}

TEST_F(AdaptiveEncodeDecode, EncodeDecodeSmallSequence) {
  auto dict0 = ael::dict::AdaptiveDictionary({8, 5});
  const auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(smallSequence, dict0).finalize();

  auto dict = ael::dict::AdaptiveDictionary({8, 5});
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict, outIter, encoded.size());

  EXPECT_TRUE(rng::equal(encoded, decoded));
}

TEST_F(AdaptiveEncodeDecode, EncodeDecodeSmallSequenceBitsLimit) {
  auto dict0 = ael::dict::AdaptiveDictionary({8, 5});
  const auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(encoded, dict0).finalize();

  auto dict1 = ael::dict::AdaptiveDictionary({8, 5});
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser, bitsCnt).decode(dict1, outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(encoded, decoded));
}

TEST_F(AdaptiveEncodeDecode, EncodeDecodeFuzz) {
  for (auto iteration : rng::iota_view(0, 15)) {
    refreshForFuzzTest();
    const std::size_t ratio = gen() % 10 + 5;  // [5..15)

    auto dict0 = ael::dict::AdaptiveDictionary({maxOrd, ratio});
    const auto [dataConstructor, wordsCount, bitsCount] =
        ArithmeticCoder().encode(encoded, dict0).finalize();

    auto dict1 = ael::dict::AdaptiveDictionary({maxOrd, ratio});
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser).decode(dict1, outIter, encoded.size());

    EXPECT_TRUE(rng::equal(encoded, decoded));
  }
}

TEST_F(AdaptiveEncodeDecode, EncodeDecodeFuzzBitsLimit) {
  for (auto iteration : rng::iota_view(0, 15)) {
    refreshForFuzzTest();
    const std::size_t ratio = gen() % 10 + 5;  // [5..15)

    auto dict0 = ael::dict::AdaptiveDictionary({maxOrd, ratio});
    const auto [dataConstructor, wordsCnt, bitsCnt] =
        ArithmeticCoder().encode(encoded, dict0).finalize();

    auto dict1 = ael::dict::AdaptiveDictionary({maxOrd, ratio});
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser, bitsCnt).decode(dict1, outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(encoded, decoded));
  }
}

}  // namespace

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
