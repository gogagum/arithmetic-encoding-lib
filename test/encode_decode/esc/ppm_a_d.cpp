#include <gtest/gtest.h>

#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/esc/arithmetic_coder.hpp>
#include <ael/esc/arithmetic_decoder.hpp>
#include <ael/esc/dictionary/ppma_dictionary.hpp>
#include <ael/esc/dictionary/ppmd_dictionary.hpp>
#include <cstdint>
#include <encode_decode_test.hpp>
#include <ranges>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,
// cppcoreguidelines-avoid-non-const-global-variables)

namespace {

using std::views::iota;

namespace rng = std::ranges;
using ael::esc::ArithmeticCoder;
using ael::esc::ArithmeticDecoder;
template <class DictT>
using EscPPMADEncodeDecodeTest = EncodeDecodeTest<DictT>;

TYPED_TEST_SUITE_P(EscPPMADEncodeDecodeTest);

TYPED_TEST_P(EscPPMADEncodeDecodeTest, EncodeEmpty) {
  const auto encoded = std::vector<std::uint64_t>{};
  auto dict = TypeParam({6, 2});
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(encoded, dict).finalize();

  EXPECT_EQ(wordsCnt, 0);
  EXPECT_EQ(bitsCnt, 2);
  EXPECT_EQ(dataConstructor->size(), 1);
}

TYPED_TEST_P(EscPPMADEncodeDecodeTest, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = TypeParam({6, 3});
  auto parser = ael::DataParser(data);
  ArithmeticDecoder(parser, 0).decode(dict, this->outIter, 0);

  EXPECT_EQ(this->decoded.size(), 0);
}

TYPED_TEST_P(EscPPMADEncodeDecodeTest, EncodeSmall) {
  auto dict = TypeParam({8, 3});
  auto [dataConstructor, wordsCount, bitsCount] =
      ArithmeticCoder().encode(this->smallSequence, dict).finalize();

  EXPECT_EQ(wordsCount, 10);
  EXPECT_GE(dataConstructor->size(), 0);
}

TYPED_TEST_P(EscPPMADEncodeDecodeTest, EncodeDecodeEmptySequence) {
  auto dict0 = TypeParam({6, 3});
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(this->encoded, dict0).finalize();

  auto dict1 = TypeParam({6, 3});
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict1, this->outIter, {wordsCnt});

  EXPECT_EQ(this->encoded.size(), this->decoded.size());
}

TYPED_TEST_P(EscPPMADEncodeDecodeTest, EncodeDecodeSmallSequence) {
  auto dict0 = TypeParam({8, 5});
  const auto [dataConstructor, wordsCnt, _] =
      ArithmeticCoder().encode(this->smallSequence, dict0).finalize();

  auto dict1 = TypeParam({8, 5});
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict1, this->outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(this->smallSequence, this->decoded));
}

TYPED_TEST_P(EscPPMADEncodeDecodeTest, EncodeDecodeSmallSequenceBitsLimit) {
  auto dict0 = TypeParam({8, 5});
  const auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(this->smallSequence, dict0).finalize();

  auto dict1 = TypeParam({8, 5});
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser, bitsCnt).decode(dict1, this->outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(this->smallSequence, this->decoded));
}

TYPED_TEST_P(EscPPMADEncodeDecodeTest, EncodesAndDecodesWithNoBitsLimit) {
  for (auto iteration : iota(0, 15)) {
    this->refreshForFuzzTest();

    auto dict0 = TypeParam({this->maxOrd, 5});
    const auto [dataConstructor, wordsCnt, _] =
        ArithmeticCoder().encode(this->encoded, dict0).finalize();

    auto dict1 = TypeParam({this->maxOrd, 5});
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser).decode(dict1, this->outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(this->encoded, this->decoded));
  }
};

TYPED_TEST_P(EscPPMADEncodeDecodeTest, EncodesAndDecodesWithBitsLimit) {
  for (auto iteration : iota(0, 15)) {
    this->refreshForFuzzTest();

    auto dict0 = TypeParam({this->maxOrd, 5});
    const auto [dataConstructor, wordsCnt, bitsCnt] =
        ArithmeticCoder().encode(this->encoded, dict0).finalize();

    auto dict1 = TypeParam({this->maxOrd, 5});
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser, bitsCnt).decode(dict1, this->outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(this->encoded, this->decoded)); 
  }
};

REGISTER_TYPED_TEST_SUITE_P(EscPPMADEncodeDecodeTest, EncodeEmpty, DecodeEmpty,
                            EncodeSmall, EncodeDecodeEmptySequence,
                            EncodeDecodeSmallSequence,
                            EncodeDecodeSmallSequenceBitsLimit,
                            EncodesAndDecodesWithNoBitsLimit,
                            EncodesAndDecodesWithBitsLimit);

using Types = ::testing::Types<ael::esc::dict::PPMADictionary,
                               ael::esc::dict::PPMDDictionary>;

INSTANTIATE_TYPED_TEST_SUITE_P(EscPPMADEncodeDecode, EscPPMADEncodeDecodeTest,
                               Types);

}  // namespace

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,
// cppcoreguidelines-avoid-non-const-global-variables)
