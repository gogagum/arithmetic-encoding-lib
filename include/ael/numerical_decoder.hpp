#ifndef AEL_NUMERICAL_DECODER_HPP
#define AEL_NUMERICAL_DECODER_HPP

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "arithmetic_decoder.hpp"
#include "dictionary/decreasing_counts_dictionary.hpp"
#include "dictionary/decreasing_on_update_dictionary.hpp"

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The NumericalDecoder class.
///
template <class SourceT>
class NumericalDecoder {
 private:
  struct CountEntry_ {
    std::uint64_t ord;
    std::uint64_t cnt;
  };

 public:
  using OptOs = std::optional<std::reference_wrapper<std::ostream>>;

 public:
  explicit NumericalDecoder(
      SourceT& source, std::size_t dictSize, std::size_t wordsCnt,
      std::size_t totalBitsLimit = std::numeric_limits<std::size_t>::max());

  template <std::output_iterator<std::uint64_t> OutIter>
  void decode(OutIter outIter, std::uint64_t maxOrd);

  template <std::output_iterator<std::uint64_t> OutIter>
  void decode(OutIter outIter, std::uint64_t maxOrd, auto wordTick,
              auto wordCountTick, auto contentTick);

 private:
  std::vector<std::uint64_t> decodeCounts_(auto&& tick);

  std::vector<std::uint64_t> decodeOrds_(std::uint64_t maxOrd, auto&& tick);

  template <std::output_iterator<std::uint64_t> OutIter>
  void decodeContent_(OutIter outIter, std::uint64_t maxOrd,
                      const std::vector<std::uint64_t>& ords,
                      const std::vector<std::uint64_t>& counts, auto&& tick);

 private:
  std::size_t dictSize_;
  std::size_t wordsCnt_;
  ArithmeticDecoder<SourceT> decoder_;
};

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
NumericalDecoder<SourceT>::NumericalDecoder(SourceT& source,
                                            std::size_t dictSize,
                                            std::size_t wordsCnt,
                                            std::size_t totalBitsLimit)
    : dictSize_{dictSize},
      wordsCnt_{wordsCnt},
      decoder_(source, totalBitsLimit) {
}

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
template <std::output_iterator<std::uint64_t> OutIter>
void NumericalDecoder<SourceT>::decode(OutIter outIter, std::uint64_t maxOrd) {
  decode(
      outIter, maxOrd, [] {}, [] {}, [] {});
}

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
template <std::output_iterator<std::uint64_t> OutIter>
void NumericalDecoder<SourceT>::decode(OutIter outIter, std::uint64_t maxOrd,
                                       auto wordTick, auto wordCountTick,
                                       auto contentTick) {
  // Decode dictionary words
  auto ords = decodeOrds_(maxOrd, wordTick);

  // Decode counts
  auto counts = decodeCounts_(wordCountTick);

  // Decode content
  decodeContent_(outIter, maxOrd, ords, counts, contentTick);
}

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
std::vector<std::uint64_t> NumericalDecoder<SourceT>::decodeOrds_(
    std::uint64_t maxOrd, auto&& tick) {
  auto dictWordsDictionary = dict::DecreasingOnUpdateDictionary(maxOrd, 1);
  auto ords = std::vector<std::uint64_t>();
  decoder_.decode(dictWordsDictionary, std::back_inserter(ords), dictSize_,
                  tick);
  return ords;
}

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
std::vector<std::uint64_t> NumericalDecoder<SourceT>::decodeCounts_(
    auto&& tick) {
  auto countsDictionary =
      dict::DecreasingCountDictionary<std::uint64_t>(dictSize_);
  auto counts = std::vector<std::uint64_t>();
  decoder_.decode(countsDictionary, std::back_inserter(counts), dictSize_,
                  tick);
  return counts;
}

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
template <std::output_iterator<std::uint64_t> OutIter>
void NumericalDecoder<SourceT>::decodeContent_(
    OutIter outIter, std::uint64_t maxOrd,
    const std::vector<std::uint64_t>& ords,
    const std::vector<std::uint64_t>& counts, auto&& tick) {
  assert(ords.size() == counts.size());
  auto contentDictInitialCounts = std::vector<CountEntry_>();
  std::transform(ords.begin(), ords.end(), counts.begin(),
                 std::back_inserter(contentDictInitialCounts),
                 [](std::uint64_t wordOrd, std::uint64_t count) -> CountEntry_ {
                   return {wordOrd, count};
                 });
  auto contentDictionary =
      dict::DecreasingOnUpdateDictionary(maxOrd, contentDictInitialCounts);
  decoder_.decode(contentDictionary, outIter, wordsCnt_, tick);
}

}  // namespace ael

#endif  // AEL_NUMERICAL_DECODER_HPP
