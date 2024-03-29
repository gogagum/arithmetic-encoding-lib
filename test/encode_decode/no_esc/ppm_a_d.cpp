#include <gtest/gtest.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/arithmetic_decoder.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/dictionary/ppma_dictionary.hpp>
#include <ael/dictionary/ppmd_dictionary.hpp>
#include <encode_decode_test.hpp>
#include <ord_generator.hpp>
#include <random>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

namespace {

namespace rng = std::ranges;
using ael::ArithmeticCoder;
using ael::ArithmeticDecoder;
template <class DictT>
using PPMADEncodeDecodeTest = EncodeDecodeTest<DictT>;

TYPED_TEST_SUITE_P(PPMADEncodeDecodeTest);

TYPED_TEST_P(PPMADEncodeDecodeTest, EncodeEmpty) {
  auto dict = TypeParam({6, 4});
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(this->encoded, dict).finalize();

  EXPECT_EQ(wordsCnt, 0);
  EXPECT_EQ(bitsCnt, 2);
  EXPECT_EQ(dataConstructor->size(), 1);
}

TYPED_TEST_P(PPMADEncodeDecodeTest, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = TypeParam({6, 3});
  auto dataParser = ael::DataParser(data);
  ArithmeticDecoder(dataParser, 0).decode(dict, this->outIter, 0);

  EXPECT_EQ(this->decoded.size(), 0);
}

TYPED_TEST_P(PPMADEncodeDecodeTest, EncodeDecodeEmptySequence) {
  auto dict0 = TypeParam({6, 6});
  auto [dataConstructor, wordsCnt, _1] =
      ArithmeticCoder().encode(this->encoded, dict0).finalize();

  auto dict1 = TypeParam({6, 6});
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict1, this->outIter, wordsCnt);

  EXPECT_EQ(this->encoded.size(), this->decoded.size());
}

TYPED_TEST_P(PPMADEncodeDecodeTest, EncodeSmall) {
  auto dict = TypeParam({8, 6});
  auto [dataConstructor, wordsCnt, bitsCount] =
      ArithmeticCoder().encode(this->smallSequence, dict).finalize();

  EXPECT_EQ(wordsCnt, 5);
  EXPECT_GE(dataConstructor->size(), 0);
}

TYPED_TEST_P(PPMADEncodeDecodeTest, EncodeDecodeSmallSequence) {
  auto dict0 = TypeParam({8, 5});
  auto [dataConstructor, wordsCnt, bitsCount] =
      ArithmeticCoder().encode(this->smallSequence, dict0).finalize();

  auto dict1 = TypeParam({8, 5});
  auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(dataParser).decode(dict1, this->outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(this->smallSequence, this->decoded));
}

TYPED_TEST_P(PPMADEncodeDecodeTest, EncodeDecodeSmallSequenceBitsLimit) {
  auto dict0 = TypeParam({8, 5});
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(this->smallSequence, dict0).finalize();

  auto dict1 = TypeParam({8, 5});
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser, bitsCnt).decode(dict1, this->outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(this->smallSequence, this->decoded));
}

TYPED_TEST_P(PPMADEncodeDecodeTest, EncodesAndDecodes) {
  for (auto iteration : rng::iota_view(0, 15)) {
    this->refreshForFuzzTest();
    const std::size_t ctxLen = this->gen() % 6;  // [0..6)

    auto dict0 = TypeParam({this->maxOrd, ctxLen});
    auto [dataConstructor, wordsCnt, _] =
        ArithmeticCoder().encode(this->encoded, dict0).finalize();

    auto dict1 = TypeParam({this->maxOrd, ctxLen});
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser).decode(dict1, this->outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(this->encoded, this->decoded));
  }
}

TYPED_TEST_P(PPMADEncodeDecodeTest, EncodesAndDecodesBitsLimit) {
  for (auto iteration : rng::iota_view(0, 15)) {
    this->refreshForFuzzTest();
    const std::size_t ctxLen = this->gen() % 6;  // [0..6)

    auto dict0 = TypeParam({this->maxOrd, ctxLen});
    auto [dataConstructor, wordsCnt, bitsCnt] =
        ArithmeticCoder().encode(this->encoded, dict0).finalize();

    auto dict1 = TypeParam({this->maxOrd, ctxLen});
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser, bitsCnt).decode(dict1, this->outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(this->encoded, this->decoded));
  }
}

REGISTER_TYPED_TEST_SUITE_P(PPMADEncodeDecodeTest, EncodeEmpty, DecodeEmpty,
                            EncodeSmall, EncodeDecodeEmptySequence,
                            EncodeDecodeSmallSequence,
                            EncodeDecodeSmallSequenceBitsLimit,
                            EncodesAndDecodes, EncodesAndDecodesBitsLimit);

using Types =
    ::testing::Types<ael::dict::PPMADictionary, ael::dict::PPMDDictionary>;

INSTANTIATE_TYPED_TEST_SUITE_P(PPMADEncodeDecode, PPMADEncodeDecodeTest, Types);

}  // namespace

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
