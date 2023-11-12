#include <gtest/gtest.h>

#include <ael/data_parser.hpp>
#include <ael/numerical_coder.hpp>
#include <ael/numerical_decoder.hpp>

#include "encode_decode_test_params.hpp"

namespace {

class NumericalEncodeDecode
    : public testing::TestWithParam<EncodeDecodeTestParams> {};

// NOLINTBEGIN(cert-err58-cpp,
// cppcoreguidelines-avoid-non-const-global-variables)

TEST_P(NumericalEncodeDecode, EncodeDecodeAndCompare) {
  const auto& params = NumericalEncodeDecode::GetParam();
  const auto& encoded = std::get<1>(params);

  auto countsMapping = ael::NumericalCoder::countWords(encoded);

  const auto encodeRes =
      ael::NumericalCoder().encode(encoded, countsMapping);

  EXPECT_EQ(encodeRes.contentWordsEncoded, encoded.size());

  auto encodedDataParser =
      ael::DataParser(encodeRes.dataConstructor->getDataSpan());

  auto decoded = std::vector<std::uint64_t>{};
  ael::NumericalDecoder(encodedDataParser, encodeRes.dictionarySize,
                        encodeRes.contentWordsEncoded, encodeRes.totalBitsCnt)
      .decode(std::back_inserter(decoded), encodeRes.maxOrd);

  EXPECT_TRUE(std::ranges::equal(encoded, decoded));
}

const auto simpleEncodeDecodeTests = std::vector<EncodeDecodeTestParams>{
    {"OneElement", {42}},
    {"FiveEqualElements", {5, 5, 5, 5, 5}},
    {"FiveDifferentElements", {2, 3, 5, 1, 4}},
    {"OneOrdWithBigDictionarySize", {4, 4, 4, 4, 4, 4}}};

INSTANTIATE_TEST_SUITE_P(SimpleNumericalEncodeDecodeTest, NumericalEncodeDecode,
                         testing::ValuesIn(simpleEncodeDecodeTests),
                         [](const auto& paramInfo) {
                           return std::get<0>(paramInfo.param);
                         });

const auto randomDifferentSizesEncodeDecodeTests =
    GenerateEncodeDecodeTests(256, 42).ofSizes({1, 4, 16, 64, 256, 1024, 4096});

INSTANTIATE_TEST_SUITE_P(
    RandomNumericalEncodeDecodeTest, NumericalEncodeDecode,
    testing::ValuesIn(randomDifferentSizesEncodeDecodeTests),
    [](const auto& paramInfo) {
      return std::get<0>(paramInfo.param);
    });

// NOLINTEND(cert-err58-cpp,
// cppcoreguidelines-avoid-non-const-global-variables)

}  // namespace
