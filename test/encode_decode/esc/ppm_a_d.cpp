#include <gtest/gtest.h>

#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/esc/arithmetic_coder.hpp>
#include <ael/esc/arithmetic_decoder.hpp>
#include <ael/esc/dictionary/ppma_dictionary.hpp>
#include <ael/esc/dictionary/ppmd_dictionary.hpp>
#include <cstdint>
#include <ord_generator.hpp>
#include <random>
#include <ranges>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,
// cppcoreguidelines-avoid-non-const-global-variables)

namespace {

namespace rng = std::ranges;
using ael::esc::ArithmeticCoder;
using ael::esc::ArithmeticDecoder;

template <class DictT>
class EncodeDecodeEscPPMAD : public testing::Test {
 public:
  static std::vector<std::uint64_t> smallSequence;
  static std::mt19937 gen;
  static auto generateEncoded(std::size_t length, std::uint32_t rng);
};

template <class DictT>
std::vector<std::uint64_t> EncodeDecodeEscPPMAD<DictT>::smallSequence{5, 3, 5,
                                                                      7, 2};

template <class DictT>
std::mt19937 EncodeDecodeEscPPMAD<DictT>::gen{42};

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
auto EncodeDecodeEscPPMAD<DictT>::generateEncoded(std::size_t length,
                                                  std::uint32_t rng) {
  auto encoded = std::vector<std::uint64_t>{};

  std::generate_n(std::back_inserter(encoded), length,
                  ael::test::OrdGenerator(gen, rng));
  return encoded;
}

TYPED_TEST_SUITE_P(EncodeDecodeEscPPMAD);

TYPED_TEST_P(EncodeDecodeEscPPMAD, EncodeEmpty) {
  const auto encoded = std::vector<std::uint64_t>();
  auto dict = TypeParam({6, 2});
  auto dataConstructor = ael::ByteDataConstructor();
  auto [wordsCount, bitsCount] =
      ael::esc::ArithmeticCoder::encode(encoded, dataConstructor, dict);

  EXPECT_EQ(wordsCount, 0);
  EXPECT_EQ(bitsCount, 2);
  EXPECT_EQ(dataConstructor.size(), 1);
}

TYPED_TEST_P(EncodeDecodeEscPPMAD, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = TypeParam({6, 3});
  auto dataParser = ael::DataParser(data);
  auto retOrds = std::vector<std::uint32_t>();
  ArithmeticDecoder::decode(dataParser, dict, std::back_inserter(retOrds),
                            {0, 0});

  EXPECT_EQ(retOrds.size(), 0);
}

TYPED_TEST_P(EncodeDecodeEscPPMAD, EncodeSmall) {
  auto dict = TypeParam({8, 3});
  auto dataConstructor = ael::ByteDataConstructor();
  auto [wordsCount, bitsCount] = ael::esc::ArithmeticCoder::encode(
      this->smallSequence, dataConstructor, dict);

  EXPECT_EQ(wordsCount, 10);
  EXPECT_GE(dataConstructor.size(), 0);
}

TYPED_TEST_P(EncodeDecodeEscPPMAD, EncodeDecodeEmptySequence) {
  const auto encoded = std::vector<std::uint64_t>();
  auto dataConstructor = ael::ByteDataConstructor();
  auto decoded = std::vector<std::uint64_t>();

  auto dict = TypeParam({6, 3});
  auto ret = ArithmeticCoder::encode(encoded, dataConstructor, dict);

  {
    auto dict = TypeParam({6, 3});
    auto dataParser = ael::DataParser(dataConstructor.getDataSpan());
    auto outIter = std::back_inserter(decoded);
    ArithmeticDecoder::decode(dataParser, dict, outIter, {ret.wordsCount});
  }

  EXPECT_EQ(encoded.size(), decoded.size());
}

TYPED_TEST_P(EncodeDecodeEscPPMAD, EncodeDecodeSmallSequence) {
  auto dataConstructor = ael::ByteDataConstructor();
  auto decoded = std::vector<std::uint64_t>();

  auto dict = TypeParam({8, 5});
  const auto ret =
      ArithmeticCoder::encode(this->smallSequence, dataConstructor, dict);

  {
    auto dict = TypeParam({8, 5});
    auto dataParser = ael::DataParser(dataConstructor.getDataSpan());
    auto outIter = std::back_inserter(decoded);
    ArithmeticDecoder::decode(dataParser, dict, outIter, {ret.wordsCount});
  }

  EXPECT_TRUE(rng::equal(this->smallSequence, decoded));
}

TYPED_TEST_P(EncodeDecodeEscPPMAD, EncodeDecodeSmallSequenceBitsLimit) {
  auto dataConstructor = ael::ByteDataConstructor();
  auto decoded = std::vector<std::uint64_t>();

  auto dict0 = TypeParam({8, 5});
  const auto [wordsCount, bitsCount] =
      ArithmeticCoder::encode(this->smallSequence, dataConstructor, dict0);

  {
    auto dict1 = TypeParam({8, 5});
    auto dataParser = ael::DataParser(dataConstructor.getDataSpan());
    auto outIter = std::back_inserter(decoded);
    ArithmeticDecoder::decode(dataParser, dict1, outIter,
                              {wordsCount, bitsCount});
  }

  EXPECT_TRUE(rng::equal(this->smallSequence, decoded));
}

TYPED_TEST_P(EncodeDecodeEscPPMAD, EncodesAndDecodesWithNoBitsLimit) {
  for (auto iteration : std::ranges::iota_view(0, 15)) {
    const std::size_t length = this->gen() % 250;
    const std::uint32_t rng = this->gen() % 256;

    auto encoded = this->generateEncoded(length, rng);
    auto dataConstructor = ael::ByteDataConstructor();
    auto decoded = std::vector<std::uint64_t>();

    auto dict = TypeParam({rng, 5});
    const auto ret = ArithmeticCoder::encode(encoded, dataConstructor, dict);

    {
      auto dict = TypeParam({rng, 5});
      auto dataParser = ael::DataParser(dataConstructor.getDataSpan());
      auto outIter = std::back_inserter(decoded);
      ArithmeticDecoder::decode(dataParser, dict, outIter, {ret.wordsCount});
    }

    EXPECT_TRUE(rng::equal(encoded, decoded));
  }
};

TYPED_TEST_P(EncodeDecodeEscPPMAD, EncodesAndDecodesWithBitsLimit) {
  for (auto iteration : std::ranges::iota_view(0, 15)) {
    const std::size_t length = this->gen() % 250;
    const std::uint32_t rng = this->gen() % 256;

    auto encoded = this->generateEncoded(length, rng);
    auto dataConstructor = ael::ByteDataConstructor();
    auto decoded = std::vector<std::uint64_t>();

    auto dict1 = TypeParam({rng, 5});
    const auto [wordsCount, bitsCount] =
        ArithmeticCoder::encode(encoded, dataConstructor, dict1);

    {
      auto dict2 = TypeParam({rng, 5});
      auto dataParser = ael::DataParser(dataConstructor.getDataSpan());
      ArithmeticDecoder::decode(dataParser, dict2, std::back_inserter(decoded),
                                {wordsCount, bitsCount});
    }

    EXPECT_TRUE(rng::equal(encoded, decoded));
  }
};

REGISTER_TYPED_TEST_SUITE_P(EncodeDecodeEscPPMAD, EncodeEmpty, DecodeEmpty,
                            EncodeSmall, EncodeDecodeEmptySequence,
                            EncodeDecodeSmallSequence,
                            EncodeDecodeSmallSequenceBitsLimit,
                            EncodesAndDecodesWithNoBitsLimit,
                            EncodesAndDecodesWithBitsLimit);

using Types = ::testing::Types<ael::esc::dict::PPMADictionary,
                               ael::esc::dict::PPMDDictionary>;

INSTANTIATE_TYPED_TEST_SUITE_P(EscPPMADEncodeDecode, EncodeDecodeEscPPMAD,
                               Types);

}  // namespace

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,
// cppcoreguidelines-avoid-non-const-global-variables)
