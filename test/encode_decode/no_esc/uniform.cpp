#include <gtest/gtest.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/arithmetic_decoder.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/dictionary/uniform_dictionary.hpp>
#include <encode_decode_test.hpp>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

namespace {

namespace rng = std::ranges;

using std::views::iota;

using ael::ArithmeticCoder;
using ael::ArithmeticDecoder;

using UniformEncodeDecode = EncodeDecodeTest<ael::dict::UniformDictionary>;

TEST_F(UniformEncodeDecode, EncodeEmpty) {
  auto dict = ael::dict::UniformDictionary(6);
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(encoded, dict).finalize();

  EXPECT_EQ(wordsCnt, 0);
  EXPECT_EQ(bitsCnt, 2);
  EXPECT_EQ(dataConstructor->size(), 1);
}

TEST_F(UniformEncodeDecode, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = ael::dict::UniformDictionary(6);
  auto dataParser = ael::DataParser(data);
  ArithmeticDecoder(dataParser, 0).decode(dict, outIter, 0);

  EXPECT_EQ(decoded.size(), 0);
}

TEST_F(UniformEncodeDecode, EncodeDecodeEmptySequence) {
  auto dict0 = ael::dict::UniformDictionary(8);
  auto [dataConstructor, wordsCnt, _] =
      ArithmeticCoder().encode(encoded, dict0).finalize();

  auto dict1 = ael::dict::UniformDictionary(8);
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict1, outIter, wordsCnt);

  EXPECT_EQ(encoded.size(), decoded.size());
}

TEST_F(UniformEncodeDecode, EncodeSmall) {
  auto dict = ael::dict::UniformDictionary(8);
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(smallSequence, dict).finalize();

  EXPECT_EQ(wordsCnt, 5);
  EXPECT_GE(dataConstructor->size(), 0);
}

TEST_F(UniformEncodeDecode, EncodeDecodeSmallSequence) {
  auto dict0 = ael::dict::UniformDictionary(8);
  const auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(smallSequence, dict0).finalize();

  auto dict = ael::dict::UniformDictionary(8);
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict, outIter, encoded.size());

  EXPECT_TRUE(rng::equal(encoded, decoded));
}

TEST_F(UniformEncodeDecode, EncodeDecodeSmallSequenceBitsLimit) {
  auto dict0 = ael::dict::UniformDictionary(8);
  const auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(encoded, dict0).finalize();

  auto dict1 = ael::dict::UniformDictionary(8);
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser, bitsCnt).decode(dict1, outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(encoded, decoded));
}

TEST_F(UniformEncodeDecode, EncodeDecodeFuzz) {
  for (auto iteration : iota(0, 15)) {
    refreshForFuzzTest();
    const std::size_t ratio = gen() % 10 + 5;  // [5..15)

    auto dict0 = ael::dict::UniformDictionary(maxOrd);
    const auto [dataConstructor, wordsCount, bitsCount] =
        ArithmeticCoder().encode(encoded, dict0).finalize();

    auto dict1 = ael::dict::UniformDictionary(maxOrd);
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser).decode(dict1, outIter, encoded.size());

    EXPECT_TRUE(rng::equal(encoded, decoded));
  }
}

TEST_F(UniformEncodeDecode, EncodeDecodeFuzzBitsLimit) {
  for (auto iteration : iota(0, 15)) {
    refreshForFuzzTest();
    const std::size_t ratio = gen() % 10 + 5;  // [5..15)

    auto dict0 = ael::dict::UniformDictionary(maxOrd);
    const auto [dataConstructor, wordsCnt, bitsCnt] =
        ArithmeticCoder().encode(encoded, dict0).finalize();

    auto dict1 = ael::dict::UniformDictionary(maxOrd);
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser, bitsCnt).decode(dict1, outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(encoded, decoded));
  }
}

}  // namespace

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
