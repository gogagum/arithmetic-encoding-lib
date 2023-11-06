#include <gtest/gtest.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/arithmetic_decoder.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/dictionary/adaptive_a_contextual_dictionary.hpp>
#include <ael/dictionary/adaptive_a_contextual_dictionary_improved.hpp>
#include <ael/dictionary/adaptive_d_contextual_dictionary.hpp>
#include <ael/dictionary/adaptive_d_contextual_dictionary_improved.hpp>
#include <encode_decode_test.hpp>
#include <random>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

namespace {

namespace rng = std::ranges;
using ael::ArithmeticCoder;
using ael::ArithmeticDecoder;
template <class DictT>
using AdaptiveADContextualEncodeDecodeTest = EncodeDecodeTest<DictT>;

TYPED_TEST_SUITE_P(AdaptiveADContextualEncodeDecodeTest);

TYPED_TEST_P(AdaptiveADContextualEncodeDecodeTest, EncodeEmpty) {
  const auto encoded = std::vector<std::uint64_t>();
  auto dict = TypeParam({6, 3, 3});
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(encoded, dict).finalize();

  EXPECT_EQ(wordsCnt, 0);
  EXPECT_EQ(bitsCnt, 2);
  EXPECT_EQ(dataConstructor->size(), 1);
}

TYPED_TEST_P(AdaptiveADContextualEncodeDecodeTest, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = TypeParam({6, 3, 3});
  auto dataParser = ael::DataParser(data);
  ArithmeticDecoder(dataParser, 0).decode(dict, this->outIter, 0);

  EXPECT_EQ(this->decoded.size(), 0);
}

TYPED_TEST_P(AdaptiveADContextualEncodeDecodeTest, EncodeDecodeEmptySequence) {
  const auto encoded = std::vector<std::uint64_t>();

  auto dict = TypeParam({6, 3, 3});
  auto [dataConstructor, wordsCnt, _] =
      ArithmeticCoder().encode(encoded, dict).finalize();

  {
    auto dict = TypeParam({6, 3, 3});
    auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(dataParser).decode(dict, this->outIter, wordsCnt);
  }

  EXPECT_EQ(encoded.size(), this->decoded.size());
}

TYPED_TEST_P(AdaptiveADContextualEncodeDecodeTest, EncodeSmall) {
  auto dict = TypeParam({11, 3, 3});
  auto [dataConstructor, wordsCnt, _] =
      ArithmeticCoder().encode(this->smallSequence, dict).finalize();

  EXPECT_EQ(wordsCnt, 5);
  EXPECT_GE(dataConstructor->size(), 0);
}

TYPED_TEST_P(AdaptiveADContextualEncodeDecodeTest, EncodeDecodeSmallSequence) {
  auto dict = TypeParam({8, 4, 4});
  auto [dataConstructor, wordsCnt, _] =
      ArithmeticCoder().encode(this->smallSequence, dict).finalize();

  {
    auto dict = TypeParam({8, 4, 4});
    auto dataParser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(dataParser).decode(dict, this->outIter, wordsCnt);
  }

  EXPECT_TRUE(rng::equal(this->smallSequence, this->decoded));
}

TYPED_TEST_P(AdaptiveADContextualEncodeDecodeTest,
             EncodeDecodeSmallSequenceBitsLimit) {
  auto dict0 = TypeParam({8, 4, 4});
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(this->smallSequence, dict0).finalize();

  {
    auto dict1 = TypeParam({8, 4, 4});
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser, bitsCnt).decode(dict1, this->outIter, wordsCnt);
  }

  EXPECT_TRUE(rng::equal(this->smallSequence, this->decoded));
}

TYPED_TEST_P(AdaptiveADContextualEncodeDecodeTest, EncodeDecodeFuzz) {
  for (auto iteration : std::ranges::iota_view(0, 15)) {
    const std::size_t length = this->gen() % 250;
    const std::uint32_t rng = this->gen() % 256;

    auto encoded = this->generateEncoded(length, rng);

    const std::uint16_t ctxLength = this->gen() % 5;      // [0..5)
    const std::uint16_t ctxBitsLength = this->gen() % 5;  // [0..5)

    auto dict = TypeParam({8, ctxLength, ctxBitsLength});
    auto [dataConstructor, wordsCnt, _] =
        ArithmeticCoder().encode(encoded, dict).finalize();

    {
      auto dict = TypeParam({8, ctxLength, ctxBitsLength});
      auto parser = ael::DataParser(dataConstructor->getDataSpan());
      ArithmeticDecoder(parser).decode(dict, this->outIter, wordsCnt);
    }

    EXPECT_TRUE(rng::equal(encoded, this->decoded));
    this->decoded.clear();
  }
}

TYPED_TEST_P(AdaptiveADContextualEncodeDecodeTest, EncodeDecodeFuzzBitsLimit) {
  for (auto iteration : std::ranges::iota_view(0, 50)) {
    const std::size_t length = this->gen() % 250;
    const std::uint32_t rng = this->gen() % 256;

    auto encoded = this->generateEncoded(length, rng);

    const std::uint16_t ctxLength = this->gen() % 5;      // [0..5)
    const std::uint16_t ctxBitsLength = this->gen() % 5;  // [0..5)

    auto dict1 = TypeParam({8, ctxLength, ctxBitsLength});
    auto [dataConstructor, wordsCnt, bitsCnt] =
        ArithmeticCoder().encode(encoded, dict1).finalize();

    {
      auto dict2 = TypeParam({8, ctxLength, ctxBitsLength});
      auto parser = ael::DataParser(dataConstructor->getDataSpan());
      ArithmeticDecoder(parser, bitsCnt).decode(dict2, this->outIter, wordsCnt);
    }

    EXPECT_TRUE(rng::equal(encoded, this->decoded));
    this->decoded.clear();
  }
}

REGISTER_TYPED_TEST_SUITE_P(AdaptiveADContextualEncodeDecodeTest, EncodeEmpty,
                            DecodeEmpty, EncodeSmall, EncodeDecodeEmptySequence,
                            EncodeDecodeSmallSequence,
                            EncodeDecodeSmallSequenceBitsLimit,
                            EncodeDecodeFuzz, EncodeDecodeFuzzBitsLimit);

using Types =
    ::testing::Types<ael::dict::AdaptiveAContextualDictionary,
                     ael::dict::AdaptiveDContextualDictionary,
                     ael::dict::AdaptiveAContextualDictionaryImproved,
                     ael::dict::AdaptiveDContextualDictionaryImproved>;

INSTANTIATE_TYPED_TEST_SUITE_P(AdaptiveADContextualEncodeDecode,
                               AdaptiveADContextualEncodeDecodeTest, Types);

}  // namespace

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
