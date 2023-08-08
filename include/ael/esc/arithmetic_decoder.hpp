#pragma once

#ifndef ESC_ARITHMETIC_DECODER_HPP
#define ESC_ARITHMETIC_DECODER_HPP

#include <ael/impl/multiply_and_divide.hpp>
#include <ael/impl/ranges_calc.hpp>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <optional>
#include <ranges>

namespace ael::esc {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class.
///
class ArithmeticDecoder {
 public:
  ////////////////////////////////////////////////////////////////////////////
  /// \brief The ArithmeticDecoder::DecodeLimits class.
  struct DecodeLimits {
    std::size_t wordsLimit;
    std::size_t bitsLimit;
  };

 public:
  /**
   * @param source - source of bits.
   * @param dict - dictionary (probability model).
   * @param outIter - output iterator for decoded sequence.
   * @param deocdeLimits - number of decoded words and number of decoded bits.
   */
  template <std::output_iterator<std::uint64_t> OutIter, class Dict>
  static void decode(auto& source, Dict& dict, OutIter outIter,
                     DecodeLimits decodeLimits);

  /**
   * @param source - source of bits.
   * @param dict - dictionary (probability model).
   * @param outIter - output iterator for decoded sequence.
   * @param wordsCount - number of decoded words.
   * @param bitsLimit - number of bits to decode.
   * @param tick - lambda to do a tick (for example, for logging).
   */
  template <std::output_iterator<std::uint64_t> OutIter, class Dict>
  static void decode(auto& source, Dict& dict, OutIter outIter,
                     DecodeLimits decodeLimits, auto tick);
};

////////////////////////////////////////////////////////////////////////////////
template <std::output_iterator<std::uint64_t> OutIter, class Dict>
void ArithmeticDecoder::decode(auto& source, Dict& dict, OutIter outIter,
                               DecodeLimits decodeLimits) {
  return decode(source, dict, outIter, decodeLimits, [] {});
}

////////////////////////////////////////////////////////////////////////////////
template <std::output_iterator<std::uint64_t> OutIter, class Dict>
void ArithmeticDecoder::decode(auto& source, Dict& dict, OutIter outIter,
                               DecodeLimits decodeLimits, auto tick) {
  using RC = impl::RangesCalc<typename Dict::Count, Dict::countNumBits>;
  const auto takeBitLimited = [&source, &decodeLimits]() -> typename RC::Count {
    if (decodeLimits.bitsLimit == 0) {
      return false;
    }
    --decodeLimits.bitsLimit;
    return source.takeBit() ? 1 : 0;
  };

  typename RC::Range currRange;
  typename RC::Count value = 0;

  for (auto _ : std::ranges::iota_view(std::size_t{0}, Dict::countNumBits)) {
    value = (value << 1) + takeBitLimited();
  }

  for (auto i :
       std::ranges::iota_view(std::size_t{0}, decodeLimits.wordsLimit)) {
    const auto range = typename Dict::Count{currRange.high - currRange.low};
    const auto dictTotalWords = dict.getTotalWordsCnt();
    const auto offset = value - currRange.low + 1;
    assert(offset < range);
    const auto aux =
        impl::multiply_decrease_and_divide(offset, dictTotalWords, range);
    const auto ord = dict.getWordOrd(aux);

    if (!dict.isEsc(ord)) {
      *outIter = ord;
      ++outIter;
    }

    auto [low, high, total] = dict.getDecodeProbabilityStats(ord);
    currRange = RC::rangeFromStatsAndPrev(currRange, {low, high, total});
    assert(high > low);

    while (true) {
      if (currRange.high <= RC::half) {
        value = value * 2 + takeBitLimited();
      } else if (currRange.low >= RC::half) {
        value = value * 2 - RC::total + takeBitLimited();
      } else if (currRange.low >= RC::quater &&
                 currRange.high <= RC::threeQuaters) {
        value = value * 2 - RC::half + takeBitLimited();
      } else {
        break;
      }
      currRange = RC::recalcRange(currRange);
    }
    tick();
  }
}

}  // namespace ael::esc

#endif  // ESC_ARITHMETIC_DECODER_HPP
