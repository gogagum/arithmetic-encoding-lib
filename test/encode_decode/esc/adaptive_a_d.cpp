#include <gtest/gtest.h>

#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/esc/arithmetic_coder.hpp>
#include <ael/esc/arithmetic_decoder.hpp>
#include <ael/esc/dictionary/adaptive_a_dictionary.hpp>
#include <ael/esc/dictionary/adaptive_d_dictionary.hpp>
#include <cstdint>
#include <encode_decode_test.hpp>
#include <random>
#include <ranges>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,
// cppcoreguidelines-avoid-non-const-global-variables)

namespace {

namespace rng = std::ranges;
using ael::esc::ArithmeticCoder;
using ael::esc::ArithmeticDecoder;
template <class DictT>
using EscAdaptiveADEncodeDecodeTest = EncodeDecodeTest<DictT>;

TYPED_TEST_SUITE_P(EscAdaptiveADEncodeDecodeTest);

TYPED_TEST_P(EscAdaptiveADEncodeDecodeTest, EncodeEmpty) {
  auto dict = TypeParam(6);
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(this->encoded, dict).finalize();

  EXPECT_EQ(wordsCnt, 0);
  EXPECT_EQ(bitsCnt, 2);
  EXPECT_EQ(dataConstructor->size(), 1);
}

TYPED_TEST_P(EscAdaptiveADEncodeDecodeTest, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = TypeParam(6);
  auto parser = ael::DataParser(data);
  ArithmeticDecoder(parser, 0).decode(dict, this->outIter, 0);

  EXPECT_EQ(this->decoded.size(), 0);
}

TYPED_TEST_P(EscAdaptiveADEncodeDecodeTest, EncodeSmall) {
  auto dict = TypeParam(8);
  auto [dataConstructor, wordsCount, bitsCount] =
      ArithmeticCoder().encode(this->smallSequence, dict).finalize();

  EXPECT_EQ(wordsCount, 9);
  EXPECT_GE(dataConstructor->size(), 0);
}

TYPED_TEST_P(EscAdaptiveADEncodeDecodeTest, EncodeDecodeEmptySequence) {
  auto dict0 = TypeParam(6);
  auto [dataConstructor, wordsCnt, _1] =
      ArithmeticCoder().encode(this->encoded, dict0).finalize();

  auto dict1 = TypeParam(6);
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict1, this->outIter, wordsCnt);

  EXPECT_EQ(this->encoded.size(), this->decoded.size());
}

TYPED_TEST_P(EscAdaptiveADEncodeDecodeTest, EncodeDecodeSmallSequence) {
  auto dict0 = TypeParam(8);
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(this->smallSequence, dict0).finalize();

  auto dict1 = TypeParam(8);
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict1, this->outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(this->smallSequence, this->decoded));
}

TYPED_TEST_P(EscAdaptiveADEncodeDecodeTest,
             EncodeDecodeSmallSequenceBitsLimit) {
  auto dict0 = TypeParam(8);
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(this->smallSequence, dict0).finalize();

  auto dict1 = TypeParam(8);
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser, bitsCnt).decode(dict1, this->outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(this->smallSequence, this->decoded));
}

TYPED_TEST_P(EscAdaptiveADEncodeDecodeTest, EncodesAndDecodesWithNoBitsLimit) {
  for (auto iteration : rng::iota_view(0, 15)) {
    this->refreshForFuzzTest();

    auto dict0 = TypeParam(this->maxOrd);
    auto [dataConstructor, wordsCnt, _] =
        ArithmeticCoder().encode(this->encoded, dict0).finalize();

    auto dict1 = TypeParam(this->maxOrd);
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser).decode(dict1, this->outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(this->encoded, this->decoded));
  }
};

TYPED_TEST_P(EscAdaptiveADEncodeDecodeTest, EncodesAndDecodesWithBitsLimit) {
  for (auto iteration : rng::iota_view(0, 15)) {
    this->refreshForFuzzTest();

    auto dict0 = TypeParam(this->maxOrd);
    auto [dataConstructor, wordsCnt, bitsCnt] =
        ArithmeticCoder().encode(this->encoded, dict0).finalize();

    auto dict1 = TypeParam(this->maxOrd);
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser, bitsCnt).decode(dict1, this->outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(this->encoded, this->decoded));
  }
};

REGISTER_TYPED_TEST_SUITE_P(EscAdaptiveADEncodeDecodeTest, EncodeEmpty,
                            DecodeEmpty, EncodeSmall, EncodeDecodeEmptySequence,
                            EncodeDecodeSmallSequence,
                            EncodeDecodeSmallSequenceBitsLimit,
                            EncodesAndDecodesWithNoBitsLimit,
                            EncodesAndDecodesWithBitsLimit);

using Types = ::testing::Types<ael::esc::dict::AdaptiveADictionary,
                               ael::esc::dict::AdaptiveDDictionary>;

INSTANTIATE_TYPED_TEST_SUITE_P(EscAdaptiveADEncodeDecode,
                               EscAdaptiveADEncodeDecodeTest, Types);

}  // namespace

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,
// cppcoreguidelines-avoid-non-const-global-variables)
