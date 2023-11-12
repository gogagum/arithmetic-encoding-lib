#include <gtest/gtest.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/arithmetic_decoder.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/dictionary/static_dictionary.hpp>
#include <encode_decode_test.hpp>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

namespace {

namespace rng = std::ranges;

using ael::ArithmeticCoder;
using ael::ArithmeticDecoder;

using StaticEncodeDecode = EncodeDecodeTest<ael::dict::StaticDictionary>;

using CountsMapping = std::vector<ael::dict::StaticDictionary::CountMapping>;

TEST_F(StaticEncodeDecode, EncodeEmpty) {
  auto dict = ael::dict::StaticDictionary(6, CountsMapping{});
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(encoded, dict).finalize();

  EXPECT_EQ(wordsCnt, 0);
  EXPECT_EQ(bitsCnt, 2);
  EXPECT_EQ(dataConstructor->size(), 1);
}

TEST_F(StaticEncodeDecode, DecodeEmpty) {
  const auto data = std::array<std::byte, 0>{};
  auto dict = ael::dict::StaticDictionary(6, CountsMapping{});
  auto dataParser = ael::DataParser(data);
  ArithmeticDecoder(dataParser, 0).decode(dict, outIter, 0);

  EXPECT_EQ(decoded.size(), 0);
}

TEST_F(StaticEncodeDecode, EncodeDecodeEmptySequence) {
  auto dict0 = ael::dict::StaticDictionary(8, CountsMapping{});
  auto [dataConstructor, wordsCnt, _] =
      ArithmeticCoder().encode(encoded, dict0).finalize();

  auto dict1 = ael::dict::StaticDictionary(8, CountsMapping{});
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict1, outIter, wordsCnt);

  EXPECT_EQ(encoded.size(), decoded.size());
}

TEST_F(StaticEncodeDecode, EncodeSmall) {
  auto dict = ael::dict::StaticDictionary(
      8, CountsMapping{{2, 1}, {5, 2}, {3, 1}, {7, 1}});
  auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(smallSequence, dict).finalize();

  EXPECT_EQ(wordsCnt, 5);
  EXPECT_GE(dataConstructor->size(), 0);
}

TEST_F(StaticEncodeDecode, EncodeDecodeSmallSequence) {
  auto cntMapping = CountsMapping{{2, 1}, {5, 2}, {3, 1}, {7, 1}};

  auto dict0 = ael::dict::StaticDictionary(8, cntMapping);
  const auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(smallSequence, dict0).finalize();

  auto dict = ael::dict::StaticDictionary(8, cntMapping);
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser).decode(dict, outIter, encoded.size());

  EXPECT_TRUE(rng::equal(encoded, decoded));
}

TEST_F(StaticEncodeDecode, EncodeDecodeSmallSequenceBitsLimit) {
  auto cntMapping = CountsMapping{{2, 1}, {5, 2}, {3, 1}, {7, 1}};

  auto dict0 = ael::dict::StaticDictionary(8, cntMapping);
  const auto [dataConstructor, wordsCnt, bitsCnt] =
      ArithmeticCoder().encode(encoded, dict0).finalize();

  auto dict1 = ael::dict::StaticDictionary(8, cntMapping);
  auto parser = ael::DataParser(dataConstructor->getDataSpan());
  ArithmeticDecoder(parser, bitsCnt).decode(dict1, outIter, wordsCnt);

  EXPECT_TRUE(rng::equal(encoded, decoded));
}

TEST_F(StaticEncodeDecode, EncodeDecodeFuzz) {
  for (auto iteration : rng::iota_view(0, 15)) {
    refreshForFuzzTest();
    const auto flatCountMapping =
        encoded | rng::views::transform([](std::uint64_t encEl) {
          return ael::dict::StaticDictionary::CountMapping{encEl, 1};
        });

    auto dict0 = ael::dict::StaticDictionary(maxOrd, flatCountMapping);
    const auto [dataConstructor, wordsCount, bitsCount] =
        ArithmeticCoder().encode(encoded, dict0).finalize();

    auto dict1 = ael::dict::StaticDictionary(maxOrd, flatCountMapping);
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser).decode(dict1, outIter, encoded.size());

    EXPECT_TRUE(rng::equal(encoded, decoded));
  }
}

TEST_F(StaticEncodeDecode, EncodeDecodeFuzzBitsLimit) {
  for (auto iteration : rng::iota_view(0, 15)) {
    refreshForFuzzTest();
    const auto flatCountMapping =
        encoded | rng::views::transform([](std::uint64_t encEl) {
          return ael::dict::StaticDictionary::CountMapping{encEl, 1};
        });

    auto dict0 = ael::dict::StaticDictionary(maxOrd, flatCountMapping);
    const auto [dataConstructor, wordsCnt, bitsCnt] =
        ArithmeticCoder().encode(encoded, dict0).finalize();

    auto dict1 = ael::dict::StaticDictionary(maxOrd, flatCountMapping);
    auto parser = ael::DataParser(dataConstructor->getDataSpan());
    ArithmeticDecoder(parser, bitsCnt).decode(dict1, outIter, wordsCnt);

    EXPECT_TRUE(rng::equal(encoded, decoded));
  }
}

}  // namespace

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
