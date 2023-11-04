#ifndef AEL_NUMERICAL_CODER_HPP
#define AEL_NUMERICAL_CODER_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <ranges>

#include "arithmetic_coder.hpp"
#include "byte_data_constructor.hpp"
#include "dictionary/decreasing_counts_dictionary.hpp"
#include "dictionary/decreasing_on_update_dictionary.hpp"

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The Numerical coder class.
///
template <class OrdFlow>
class NumericalCoder {
 public:
  struct EncodeRet {
    std::unique_ptr<ByteDataConstructor> dataConstructor;
    std::uint64_t dictionarySize{};
    std::uint64_t dictionaryWordsBitsCnt{};
    std::uint64_t wordsCountsBitsCnt{};
    std::uint64_t contentWordsEncoded{};
    std::uint64_t contentBitsCnt{};
  };

  struct CountEntry {
    std::uint64_t ord{};
    std::uint64_t count{};
  };

 public:
  explicit NumericalCoder(
      const OrdFlow& ordFlow,
      std::unique_ptr<ByteDataConstructor>&& dataConstructor =
          std::make_unique<ByteDataConstructor>());

  static std::vector<CountEntry> countWords(const auto& ordFlow);

  EncodeRet encode(const std::vector<CountEntry>& countsMapping) &&;

  EncodeRet encode(const std::vector<CountEntry>& countsMapping, auto wordTick,
                   auto wordCntTick, auto contentTick) &&;

 private:
  EncodeRet encode_(const std::vector<CountEntry>& countsMapping, auto wordTick,
                    auto wordCntTick, auto contentTick) &&;

 private:
  struct BitsCountsPositions_ {
    std::size_t countsBitsPos{};
    std::size_t wordsBitsPos{};
    std::size_t contentBitsPos{};
  };

 private:
  const OrdFlow* ordFlow_;
  std::unique_ptr<ByteDataConstructor> dataConstructor_;
};

////////////////////////////////////////////////////////////////////////////////
template <class OrdFlow>
NumericalCoder<OrdFlow>::NumericalCoder(
    const OrdFlow& ordFlow,
    std::unique_ptr<ByteDataConstructor>&& dataConstructor)
    : ordFlow_{&ordFlow}, dataConstructor_{std::move(dataConstructor)} {
}

////////////////////////////////////////////////////////////////////////////////
template <class OrdFlow>
auto NumericalCoder<OrdFlow>::encode(
    const std::vector<CountEntry>& countsMapping) && -> EncodeRet {
  return std::move(*this).encode_(
      countsMapping, [] {}, [] {}, [] {});
}

////////////////////////////////////////////////////////////////////////////////
template <class OrdFlow>
auto NumericalCoder<OrdFlow>::encode(
    const std::vector<CountEntry>& countsMapping, auto wordTick,
    auto wordCntTick, auto contentTick) && -> EncodeRet {
  return encode_(countsMapping, wordTick, wordCntTick, contentTick);
}

////////////////////////////////////////////////////////////////////////////////
template <class OrdFlow>
auto NumericalCoder<OrdFlow>::encode_(
    const std::vector<CountEntry>& countsMapping, auto wordTick,
    auto wordCntTick, auto contentTick) && -> EncodeRet {
  if (ordFlow_->size() == 0) {
    return {std::move(dataConstructor_), 0, 0, 0};
  }

  auto counts = std::vector<std::uint64_t>{};
  auto dictWordsOrds = std::vector<std::uint64_t>{};

  for (auto [ord, count] : countsMapping) {
    counts.push_back(count);
    dictWordsOrds.push_back(ord);
  }

  const auto maxOrd = *std::ranges::max_element(*ordFlow_) + 1;
  auto arithmeticCoder = ArithmeticCoder(std::move(dataConstructor_));

  // Encode words
  auto wordsDict = dict::DecreasingOnUpdateDictionary(maxOrd, 1);
  auto [wordsEncoded, wordsBitsCnt] =
      arithmeticCoder.encode(dictWordsOrds, wordsDict, wordTick)
          .getStatsChange();
  assert(wordsEncoded == dictWordsOrds.size());

  // Encode counts
  auto countsDict =
      dict::DecreasingCountDictionary<std::uint64_t>(ordFlow_->size());
  auto [countsEncoded, countsBitsCnt] =
      arithmeticCoder.encode(counts, countsDict, wordCntTick).getStatsChange();
  assert(countsEncoded == counts.size());

  // Encode content
  auto contentDict = dict::DecreasingOnUpdateDictionary(maxOrd, countsMapping);
  auto [contentWordsEncoded, contentBitsCnt] =
      arithmeticCoder.encode(*ordFlow_, contentDict, contentTick)
          .getStatsChange();
  assert(contentWordsEncoded == ordFlow_->size());

  auto [dataConstructor, totalWordsEncoded, totalBitsEncoded] =
      std::move(arithmeticCoder).finalize();

  return {std::move(dataConstructor),
          countsMapping.size(),
          wordsBitsCnt,
          countsBitsCnt,
          ordFlow_->size(),
          contentBitsCnt};
}

////////////////////////////////////////////////////////////////////////////////
template <class OrdFlow>
auto NumericalCoder<OrdFlow>::countWords(const auto& ordFlow)
    -> std::vector<CountEntry> {
  auto countsMap = std::map<std::uint64_t, std::uint64_t>();
  for (auto ord : ordFlow) {
    ++countsMap[ord];
  }
  auto ret = std::vector<CountEntry>{};
  std::transform(countsMap.begin(), countsMap.end(), std::back_inserter(ret),
                 [](auto entry) {
                   return CountEntry{entry.first, entry.second};
                 });
  std::sort(ret.begin(), ret.end(), [](const auto& entry0, const auto& entry1) {
    return entry0.count > entry1.count;
  });  // TODO(gogagum):  do with ranges::sort
  return ret;
}

}  // namespace ael

#endif  // AEL_NUMERICAL_CODER_HPP
