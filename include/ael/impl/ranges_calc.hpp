#ifndef RANGES_CALC_HPP
#define RANGES_CALC_HPP

#include "multiply_and_divide.hpp"
#include <cstdint>
#include <iostream>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The RangesCalc class
///
template <class CountT, std::uint16_t numBits>
class RangesCalc {
public:

    using Count = CountT;
    struct Range;

    constexpr static const Count total = Count{1} << numBits;
    constexpr static const Count half = Count{1} << (numBits - 1);
    constexpr static const Count quater = Count{1} << (numBits - 2);
    constexpr static const Count threeQuaters = 3 * quater;

    static Range recalcRange(Range rng);

    static Range rangeFromStatsAndPrev(
            Range rng, Count low, Count high, Count totalCnt);

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
        return { rng.low * 2, rng.high * 2 };
    }
    if (rng.low >= half) {
        return { rng.low * 2 - total, rng.high * 2 - total };
    } 
    if (rng.low >= quater && rng.high <= threeQuaters) {
        return { rng.low * 2 - half, rng.high * 2 - half };
    }
    return rng;
}

////////////////////////////////////////////////////////////////////////////////
template <class CountT, std::uint16_t numBits>
auto RangesCalc<CountT, numBits>::rangeFromStatsAndPrev(
        Range rng, Count low, Count high, Count totalCnt) -> Range {
    const auto range = Count(rng.high - rng.low);

    const auto lowScaled = multiply_and_divide(range, low, totalCnt);
    const auto highScaled = multiply_and_divide(range, high, totalCnt);

    return { rng.low + lowScaled, rng.low + highScaled };
}

}  // namespace ael::impl

#endif // RANGES_CALC_HPP
