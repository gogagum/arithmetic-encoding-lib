#ifndef AEL_ARITHMETIC_DECODER_HPP
#define AEL_ARITHMETIC_DECODER_HPP

#include <ael/impl/multiply_and_divide.hpp>
#include <ael/impl/range_and_value_save_base.hpp>
#include <ael/impl/ranges_calc.hpp>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ranges>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class.
///
template <class SourceT>
class ArithmeticDecoder
    : impl::RangeAndValueSaveBase<ArithmeticDecoder<SourceT>> {
 public:
  explicit ArithmeticDecoder(
      SourceT& source,
      std::size_t bitsLimit = std::numeric_limits<std::size_t>::max());

  /**
   * @param dict - dictionary (probability model).
   * @param outIter - output iterator for decoded sequence.
   * @param wordsLimit - number of words to decode.
   */
  template <std::output_iterator<std::uint64_t> OutIter, class Dict>
  void decode(Dict& dict, OutIter outIter, std::size_t wordsLimit);

  /**
   * @param dict - dictionary (probability model).
   * @param outIter - output iterator for decoded sequence.
   * @param wordsLimit - number of words to decode.
   * @param tick - lambda to do a tick (for example, for logging).
   */
  template <std::output_iterator<std::uint64_t> OutIter, class Dict>
  void decode(Dict& dict, OutIter outIter, std::size_t wordsLimit, auto tick);

 private:
  template <typename CountT>
  CountT takeBit_();

 private:
  SourceT* source_;
  const std::size_t bitsLimit_;
  std::size_t bitsDecoded_{};

 private:
  friend class impl::RangeAndValueSaveBase<ArithmeticDecoder<SourceT>>;
};

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
inline ArithmeticDecoder<SourceT>::ArithmeticDecoder(SourceT& source,
                                                     std::size_t bitsLimit)
    : source_{&source}, bitsLimit_{bitsLimit} {
}

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
template <std::output_iterator<std::uint64_t> OutIter, class Dict>
void ArithmeticDecoder<SourceT>::decode(Dict& dict, OutIter outIter,
                                        std::size_t wordsLimit) {
  return decode(dict, outIter, wordsLimit, [] {});
}

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
template <std::output_iterator<std::uint64_t> OutIter, class Dict>
void ArithmeticDecoder<SourceT>::decode(Dict& dict, OutIter outIter,
                                        std::size_t wordsLimit, auto tick) {
  using RC = impl::RangesCalc<typename Dict::Count, Dict::countNumBits>;
  auto currRange = this->template calcRange_<RC>();
  auto value = this->template calcValue_<RC>();

  for (auto i : std::ranges::iota_view(std::size_t{0}, wordsLimit)) {
    const auto range = typename Dict::Count{currRange.high - currRange.low};
    const auto dictTotalWords = dict.getTotalWordsCnt();
    const auto offset = value - currRange.low + 1;
    const auto aux =
        impl::multiply_decrease_and_divide(offset, dictTotalWords, range);
    const auto ord = dict.getWordOrd(aux);
    *outIter = ord;
    ++outIter;

    auto [low, high, total] = dict.getProbabilityStats(ord);
    currRange = RC::rangeFromStatsAndPrev(currRange, {low, high, total});

    while (true) {
      if (currRange.high <= RC::half) {
        value = value * 2 + takeBit_<typename RC::Count>();
      } else if (currRange.low >= RC::half) {
        value = value * 2 - RC::total + takeBit_<typename RC::Count>();
      } else if (currRange.low >= RC::quarter &&
                 currRange.high <= RC::threeQuarters) {
        value = value * 2 - RC::half + takeBit_<typename RC::Count>();
      } else {
        break;
      }
      currRange = RC::recalcRange(currRange);
    }
    tick();
  }

  this->prevRange_.low = currRange.low;
  this->prevRange_.high = currRange.high;
  this->prevRange_.total = RC::total;
  this->prevValue_ = value;
}

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
template <class CountT>
CountT ArithmeticDecoder<SourceT>::takeBit_() {
  if (bitsDecoded_ == bitsLimit_) {
    return 0;
  }
  ++bitsDecoded_;
  return source_->takeBit() ? 1 : 0;
}

}  // namespace ael

#endif  // AEL_ARITHMETIC_DECODER_HPP
