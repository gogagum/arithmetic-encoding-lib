#ifndef RANGES_CALC_HPP
#define RANGES_CALC_HPP

#include "ael/impl/multiply_and_divide.hpp"
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

public:
    constexpr static const Count total = Count{1} << numBits;
    constexpr static const Count half = Count{1} << (numBits - 1);
    constexpr static const Count quater = Count{1} << (numBits - 2);
    constexpr static const Count threeQuaters = 3 * quater;

public:

    static Range recalcRange(Range r);

    static Range rangeFromStatsAndPrev(
            Range r, Count low, Count high, Count total);

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
auto RangesCalc<CountT, numBits>::recalcRange(Range r) -> Range {
    if (r.high <= half) {
        return { r.low * 2, r.high * 2 };
    } else if (r.low >= half) {
        return { r.low * 2 - total, r.high * 2 - total };
    } else if (r.low >= quater && r.high <= threeQuaters) {
        return { r.low * 2 - half, r.high * 2 - half };
    }
    return r;
}

////////////////////////////////////////////////////////////////////////////////
template <class CountT, std::uint16_t numBits>
auto RangesCalc<CountT, numBits>::rangeFromStatsAndPrev(
        Range r, Count low, Count high, Count total) -> Range {
    const auto range = Count(r.high - r.low);

    const auto lowScaled = multiply_and_divide(range, low, total);
    const auto highScaled = multiply_and_divide(range, high, total);

    return { r.low + lowScaled, r.low + highScaled };
}

}  // namespace ael::impl

#endif // RANGES_CALC_HPP
