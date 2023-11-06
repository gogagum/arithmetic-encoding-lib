#include <gtest/gtest.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/arithmetic_decoder.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/dictionary/adaptive_a_dictionary.hpp>
#include <ael/dictionary/adaptive_d_dictionary.hpp>
#include <encode_decode_test.hpp>
#include <random>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

namespace {

namespace rng = std::ranges;
using ael::ArithmeticCoder;
using ael::ArithmeticDecoder;
template <class DictT>
using AdaptiveADEncodeDecodeTest = EncodeDecodeTest<DictT>;

TYPED_TEST_SUITE_P(AdaptiveADEncodeDecodeTest);

TYPED_TEST_P(AdaptiveADEncodeDecodeTest, EncodeEmpty) {
  const auto encoded = std::vector<std::uint64_t>{};
  auto dict = TypeParam(6);
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(encoded, dict).finalize();

  EXPECT_EQ(wordsCnt, 0);
  EXPECT_EQ(bitsCnt, 2);
  EXPECT_EQ(dataConstructor->size(), 1);
}

TYPED_TEST_P(AdaptiveADEncodeDecodeTest, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = TypeParam(6);
  auto dataParser = ael::DataParser(data);
  ArithmeticDecoder(dataParser, 0).decode(dict, this->outIter, 0);

  EXPECT_EQ(this->decoded.size(), 0);
}

TYPED_TEST_P(AdaptiveADEncodeDecodeTest, EncodeDecodeEmptySequence) {
  const auto encoded = std::vector<std::uint64_t>{};

  auto dict0 = TypeParam(6);
  auto [dataConstructor, wordsCnt, _] =
      ArithmeticCoder().encode(encoded, dict0).finalize();

  auto dict1 = TypeParam(6);
  auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(dataParser).decode(dict1, this->outIter, wordsCnt);

  EXPECT_EQ(this->decoded.size(), 0);
}

TYPED_TEST_P(AdaptiveADEncodeDecodeTest, EncodeSmall) {
  auto dict = TypeParam(11);
  auto [dataConstructor, wordsCnt, _] =
      ArithmeticCoder().encode(this->smallSequence, dict).finalize();

  EXPECT_EQ(wordsCnt, 5);
  EXPECT_GE(dataConstructor->size(), 0);
}

TYPED_TEST_P(AdaptiveADEncodeDecodeTest, EncodeDecodeSmallSequence) {
  auto dict0 = TypeParam(8);
  auto [dataConstructor, wordsCnt, _] =
      ArithmeticCoder().encode(this->smallSequence, dict0).finalize();

  auto dict1 = TypeParam(8);
  auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(dataParser).decode(dict1, this->outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(this->smallSequence, this->decoded));
}

TYPED_TEST_P(AdaptiveADEncodeDecodeTest, EncodeDecodeSmallSequenceBitsLimit) {
  auto dict0 = TypeParam(8);
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(this->smallSequence, dict0).finalize();

  auto dict1 = TypeParam(8);
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser, bitsCnt).decode(dict1, this->outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(this->smallSequence, this->decoded));
}

TYPED_TEST_P(AdaptiveADEncodeDecodeTest, EncodeDecodeFuzz) {
  for (auto iteration : rng::iota_view(0, 15)) {
    const std::uint32_t rng = this->gen() % 256;
    const auto encoded =
        this->generateEncoded(this->gen() % 250 /*length*/, rng);

    auto dict0 = TypeParam(rng);
    const auto [dataConstructor, wordsCnt, _] =
        ArithmeticCoder().encode(encoded, dict0).finalize();

    auto dict1 = TypeParam(rng);
    auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(dataParser).decode(dict1, this->outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(encoded, this->decoded));
    this->decoded.clear();
  }
}

TYPED_TEST_P(AdaptiveADEncodeDecodeTest, EncodesAndDecodesBitsLimit) {
  for (auto iteration : rng::iota_view(0, 15)) {
    const std::uint32_t rng = this->gen() % 256;
    const auto encoded =
        this->generateEncoded(this->gen() % 250 /*length*/, rng);

    auto dict0 = TypeParam(rng);
    const auto [dataConstructor, wordsCnt, bitsCnt] =
        ArithmeticCoder().encode(encoded, dict0).finalize();

    auto dict1 = TypeParam(rng);
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser, bitsCnt).decode(dict1, this->outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(encoded, this->decoded));
    this->decoded.clear();
  }
}

REGISTER_TYPED_TEST_SUITE_P(AdaptiveADEncodeDecodeTest, EncodeEmpty,
                            DecodeEmpty, EncodeSmall, EncodeDecodeEmptySequence,
                            EncodeDecodeSmallSequence,
                            EncodeDecodeSmallSequenceBitsLimit,
                            EncodeDecodeFuzz, EncodesAndDecodesBitsLimit);

using Types = ::testing::Types<ael::dict::AdaptiveADictionary,
                               ael::dict::AdaptiveDDictionary>;

INSTANTIATE_TYPED_TEST_SUITE_P(AdaptiveADEncodeDecode,
                               AdaptiveADEncodeDecodeTest, Types);

}  // namespace

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
