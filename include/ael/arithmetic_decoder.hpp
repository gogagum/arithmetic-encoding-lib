#ifndef AEL_ARITHMETIC_DECODER_HPP
#define AEL_ARITHMETIC_DECODER_HPP

#include <ael/impl/multiply_and_divide.hpp>
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
class ArithmeticDecoder {
 public:
  explicit ArithmeticDecoder(
      SourceT& source,  // TODO(gogagum): maybe take by unique_ptr
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
  template <class RC>
  RC::Range calcRange_();

  template <typename RC>
  RC::Count calcValue_();

 private:
  using WideNum_ = boost::multiprecision::uint256_t;

  struct TmpRange_ {
    WideNum_ low;
    WideNum_ high;
    WideNum_ total;
  };

 private:
  template <typename CountT>
  CountT takeBit_();

 private:
  SourceT* source_;
  const std::size_t bitsLimit_;
  std::size_t bitsDecoded_{};
  TmpRange_ prevRange_{0, 1, 1};
  WideNum_ prevValue_;
  bool firstDecode_{true};
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
  auto currRange = calcRange_<RC>();
  auto value = calcValue_<RC>();

  for (auto i : std::ranges::iota_view(std::size_t{0}, wordsLimit)) {
    const auto range = typename Dict::Count{currRange.high - currRange.low};
    const auto dictTotalWords = dict.getTotalWordsCnt();
    const auto offset = value - currRange.low;
    assert(offset < range);
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

  prevRange_.low = currRange.low;
  prevRange_.high = currRange.high;
  prevRange_.total = RC::total;
  prevValue_ = value;
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

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
template <class RC>
auto ArithmeticDecoder<SourceT>::calcRange_() -> RC::Range {
  auto retLow = impl::multiply_and_divide(prevRange_.low, WideNum_{RC::total},
                                          prevRange_.total);
  auto retHigh = impl::multiply_decrease_and_divide(
                     prevRange_.high, WideNum_{RC::total}, prevRange_.total) +
                 1;
  return {static_cast<RC::Count>(retLow), static_cast<RC::Count>(retHigh)};
}

////////////////////////////////////////////////////////////////////////////////
template <class SourceT>
template <class RC>
auto ArithmeticDecoder<SourceT>::calcValue_() -> RC::Count {
  if (firstDecode_) {
    auto ret = typename RC::Count{0};
    for (auto _ : std::ranges::iota_view(std::size_t{0}, RC::numBits)) {
      ret = (ret << 1) + takeBit_<typename RC::Count>();
    }
    firstDecode_ = false;
    return ret;
  }
  auto ret = impl::multiply_and_divide(prevValue_, WideNum_{RC::total},
                                       prevRange_.total);
  return static_cast<RC::Count>(ret);
}

}  // namespace ael

#endif  // AEL_ARITHMETIC_DECODER_HPP
