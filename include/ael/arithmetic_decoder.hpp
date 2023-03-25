#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <boost/range/irange.hpp>

#include <cstddef>
#include <iostream>
#include <cstdint>
#include <limits>
#include <optional>
#include <iterator>

#include "impl/ranges_calc.hpp"
#include "impl/multiply_and_divide.hpp"

namespace ael {

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
    static void decode(
            auto& source,
            Dict& dict,
            OutIter outIter,
            DecodeLimits decodeLimits
        );

    /**
     * @param source - source of bits.
     * @param dict - dictionary (probability model).
     * @param outIter - output iterator for decoded sequence.
     * @param wordsCount - number of decoded words.
     * @param bitsLimit - number of bits to decode.
     * @param tick - lambda to do a tick (for example, for logging).
     */
    template <std::output_iterator<std::uint64_t> OutIter, class Dict>
    static void decode(
            auto& source,
            Dict& dict,
            OutIter outIter,
            DecodeLimits decodeLimits,
            auto tick
        );
};

////////////////////////////////////////////////////////////////////////////////
template <std::output_iterator<std::uint64_t> OutIter, class Dict>
void ArithmeticDecoder::decode(auto& source,
                               Dict& dict,
                               OutIter outIter,
                               DecodeLimits decodeLimits) {
    return decode(source, dict, outIter, decodeLimits, []{});
}

////////////////////////////////////////////////////////////////////////////////
template <std::output_iterator<std::uint64_t> OutIter, class Dict>
void ArithmeticDecoder::decode(
        auto& source,
        Dict& dict,
        OutIter outIter,
        DecodeLimits decodeLimits,
        auto tick) {
    const auto takeBitLimited = [&source, &decodeLimits]() -> bool {
        if (decodeLimits.bitsLimit == 0) {
            return false;
        }
        --decodeLimits.bitsLimit;
        return source.takeBit();
    };

    using RC = impl::RangesCalc<typename Dict::Count, Dict::countNumBits>;
    typename RC::Range currRange;
    typename RC::Count value = 0;

    for (auto _ : boost::irange<std::size_t>(0, Dict::countNumBits)) {
        value = (value << 1) + (takeBitLimited() ? 1 : 0);
    }

    for (auto i : boost::irange<std::size_t>(0, decodeLimits.wordsLimit)) {
        const auto range =
            typename Dict::Count{currRange.high - currRange.low};
        const auto dictTotalWords = dict.getTotalWordsCnt();
        const auto offset = value - currRange.low + 1;
        assert(offset < range);
        const auto aux =
            impl::multiply_decrease_and_divide(offset, dictTotalWords, range);
        const auto ord = dict.getWordOrd(aux);
        *outIter = ord;
        ++outIter;

        auto [low, high, total] = dict.getProbabilityStats(ord);
        currRange = RC::rangeFromStatsAndPrev(currRange, low, high, total);

        while (true) {
            if (currRange.high <= RC::half) {
                const bool bit = takeBitLimited();
                value = value * 2 + (bit ? 1 : 0);
            } else if (currRange.low >= RC::half) {
                const bool bit = takeBitLimited();
                value = value * 2 - RC::total + (bit ? 1 : 0);
            } else if (currRange.low >= RC::quater
                       && currRange.high <= RC::threeQuaters) {
                const bool bit = takeBitLimited();
                value = value * 2 - RC::half + (bit ? 1 : 0);
            } else {
                break;
            }
            currRange = RC::recalcRange(currRange);
        }
        tick();
    }
}

}  // namespace ael

#endif // ARITHMETIC_DECODER_HPP

