#ifndef DECREASING_COUNTS_DICTIONARY_HPP
#define DECREASING_COUNTS_DICTIONARY_HPP

#include <stdexcept>

#include "word_probability_stats.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecreasingCountDictionary class
///
template <typename CountT>
class DecreasingCountDictionary {
public:
    using Ord = CountT;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<Count>;
    constexpr const static std::uint16_t countNumBits = 62; 
public:

    /**
     * @brief DecreasingCountDictionary constructor.
     * @param initialCount - intintal count.
     */
    DecreasingCountDictionary(Count initialCount);

    /**
     * @brief getWord
     * @param cumulativeNumFound
     * @return word.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word - number to get stats for.
     * @return [low, high, total] counts.
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief getTotalWordsCount - current number of words.
     * @return number of words. In fact, it is last decoded/encoded word.
     */
    [[nodiscard]] Count getTotalWordsCnt() const { return _currentCount; }

private:
    Count _currentCount;
};

////////////////////////////////////////////////////////////////////////////////
template <typename CountT>
DecreasingCountDictionary<CountT>::DecreasingCountDictionary(Count initialCount)
    : _currentCount(initialCount) {}

////////////////////////////////////////////////////////////////////////////////
template <typename CountT>
auto DecreasingCountDictionary<CountT>::getWordOrd(
        Count cumulativeNumFound) const -> Ord {
    assert (cumulativeNumFound <= _currentCount && "Non decreasing word!");
    return cumulativeNumFound + 1;
}

////////////////////////////////////////////////////////////////////////////////
template <typename CountT>
auto DecreasingCountDictionary<CountT>::getProbabilityStats(
        Ord ord) -> ProbabilityStats {
    assert(ord <= _currentCount && "Non decreasing word!");
    const auto ret = ProbabilityStats{ ord - 1, ord, _currentCount };
    _currentCount = ord;
    return ret;
}

}  // namespace ael::dict

#endif // DECREASING_COUNTS_DICTIONARY_HPP

