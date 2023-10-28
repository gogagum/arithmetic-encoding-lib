#ifndef AEL_IMPL_RANGES_CALC_HPP
#define AEL_IMPL_RANGES_CALC_HPP

#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <cstdint>
#include <iostream>

#include "multiply_and_divide.hpp"

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The RangesCalc class
///
template <class CountT, std::uint16_t numBits>
class RangesCalc {
 public:
  using Count = CountT;
  struct Range;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;

  constexpr static const Count total = Count{1} << numBits;
  constexpr static const Count half = Count{1} << (numBits - 1);
  constexpr static const Count quarter = Count{1} << (numBits - 2);
  constexpr static const Count threeQuarters = 3 * quarter;

  static Range recalcRange(Range rng);

  static Range rangeFromStatsAndPrev(Range rng, ProbabilityStats probStats);
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The Range class
///
template <class CountT, std::uint16_t numBits>
struct RangesCalc<CountT, numBits>::Range {
  Count low = Count{0};
  Count high = total;
};

////////////////////////////////////////////////////////////////////////////////
template <class CountT, std::uint16_t numBits>
auto RangesCalc<CountT, numBits>::recalcRange(Range rng) -> Range {
  if (rng.high <= half) {
    return {rng.low * 2, rng.high * 2};
  }
  if (rng.low >= half) {
    return {rng.low * 2 - total, rng.high * 2 - total};
  }
  if (rng.low >= quarter && rng.high <= threeQuarters) {
    return {rng.low * 2 - half, rng.high * 2 - half};
  }
  return rng;
}

////////////////////////////////////////////////////////////////////////////////
template <class CountT, std::uint16_t numBits>
auto RangesCalc<CountT, numBits>::rangeFromStatsAndPrev(
    Range rng, ProbabilityStats probStats) -> Range {
  const auto range = rng.high - rng.low;

  const auto lowScaled =
      multiply_and_divide(range, probStats.low, probStats.total);
  const auto highScaled =
      multiply_and_divide(range, probStats.high, probStats.total);

  return {rng.low + lowScaled, rng.low + highScaled};
}

}  // namespace ael::impl

#endif  // AEL_IMPL_RANGES_CALC_HPP
