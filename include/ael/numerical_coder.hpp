#ifndef AEL_NUMERICAL_CODER_HPP
#define AEL_NUMERICAL_CODER_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <ranges>

#include "arithmetic_coder.hpp"
#include "byte_data_constructor.hpp"
#include "dictionary/decreasing_counts_dictionary.hpp"
#include "dictionary/decreasing_on_update_dictionary.hpp"

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The Numerical coder class.
///
class NumericalCoder {
 public:
  struct EncodeRet {
    std::uint64_t dictSize{};
    std::uint64_t wordsBitsCnt{};
    std::uint64_t wordsCountsBitsCnt{};
    std::uint64_t contentWordsEncoded{};
    std::uint64_t contentBitsCnt{};
  };

  struct CountEntry {
    std::uint64_t ord{};
    std::uint64_t count{};
  };

 public:
  static std::vector<CountEntry> countWords(const auto& ordFlow);

  static EncodeRet encode(auto& ordFlow,
                          const std::vector<CountEntry>& countsMapping,
                          ByteDataConstructor& dataConstructor);

  static EncodeRet encode(auto& ordFlow,
                          const std::vector<CountEntry>& countsMapping,
                          ByteDataConstructor& dataConstructor, auto wordTick,
                          auto wordCntTick, auto contentTick);

 private:
  static EncodeRet encode_(auto& ordFlow,
                           const std::vector<CountEntry>& countsMapping,
                           ByteDataConstructor& dataConstructor, auto wordTick,
                           auto wordCntTick, auto contentTick);

 private:
  struct BitsCountsPositions_ {
    std::size_t countsBitsPos{};
    std::size_t wordsBitsPos{};
    std::size_t contentBitsPos{};
  };
};

////////////////////////////////////////////////////////////////////////////////
auto NumericalCoder::encode(auto& ordFlow,
                            const std::vector<CountEntry>& countsMapping,
                            ByteDataConstructor& dataConstructor) -> EncodeRet {
  return encode_(
      ordFlow, countsMapping, dataConstructor, [] {}, [] {}, [] {});
}

////////////////////////////////////////////////////////////////////////////////
auto NumericalCoder::encode(auto& ordFlow,
                            const std::vector<CountEntry>& countsMapping,
                            ByteDataConstructor& dataConstructor, auto wordTick,
                            auto wordCntTick, auto contentTick) -> EncodeRet {
  return encode_(ordFlow, countsMapping, dataConstructor, wordTick, wordCntTick,
                 contentTick);
}

////////////////////////////////////////////////////////////////////////////////
auto NumericalCoder::encode_(auto& ordFlow,
                             const std::vector<CountEntry>& countsMapping,
                             ByteDataConstructor& dataConstructor,
                             auto wordTick, auto wordCntTick, auto contentTick)
    -> EncodeRet {
  if (ordFlow.size() == 0) {
    return {0, 0, 0, 0, 0};
  }

  auto counts = std::vector<std::uint64_t>{};
  auto dictWordsOrds = std::vector<std::uint64_t>{};

  for (auto [ord, count] : countsMapping) {
    counts.push_back(count);
    dictWordsOrds.push_back(ord);
  }

  const auto maxOrd = *std::ranges::max_element(ordFlow) + 1;

  // Encode words
  auto wordsDict = dict::DecreasingOnUpdateDictionary(maxOrd, 1);
  auto [dictWordsEncoded, wordsBitsCnt] = ArithmeticCoder::encode(
      dictWordsOrds, dataConstructor, wordsDict, wordTick);
  assert(dictWordsEncoded == countsMapping.size());

  // Encode counts
  auto countsDict =
      dict::DecreasingCountDictionary<std::uint64_t>(ordFlow.size());
  auto [dictWordsCountsEncoded, countsBitsCnt] =
      ArithmeticCoder::encode(counts, dataConstructor, countsDict, wordCntTick);
  assert(dictWordsCountsEncoded == countsMapping.size());

  // Encode content
  auto contentDict = dict::DecreasingOnUpdateDictionary(maxOrd, countsMapping);
  auto [contentWordsEncoded, contentBitsCnt] = ArithmeticCoder::encode(
      ordFlow, dataConstructor, contentDict, contentTick);
  assert(contentWordsEncoded == ordFlow.size());

  return {countsMapping.size(), wordsBitsCnt, countsBitsCnt, ordFlow.size(),
          contentBitsCnt};
}

////////////////////////////////////////////////////////////////////////////////
auto NumericalCoder::countWords(const auto& ordFlow)
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
  });
  return ret;
}

}  // namespace ael

#endif  // AEL_NUMERICAL_CODER_HPP
