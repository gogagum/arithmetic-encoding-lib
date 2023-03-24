#ifndef STATIC_DICTIONARY_HPP
#define STATIC_DICTIONARY_HPP

#include "word_probability_stats.hpp"

#include <vector>
#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The StaticDictionary class
///
class StaticDictionary {
public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;

public:

    /**
     * @brief fromCounts construct from simple wordsCounts
     * @param countsRng - count of each symbol ordered.
     * @return
     */
    template <class RangeT>
    StaticDictionary(Ord maxOrd, const RangeT& countsRng);

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getProbabilityStats - get lower cumulative number of words,
     * higher cumulative number of words and total count.
     * @param word - word to get info for.
     * @return statistics.
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief totalWordsCount
     * @return
     */
    [[nodiscard]] Ord
    getTotalWordsCount() const { return *_cumulativeNumFound.rbegin(); }

private:

    Count _getLowerCumulativeCnt(Ord ord) const;

    Count
    _getHigherCumulativeCnt(Ord ord) const { return _cumulativeNumFound[ord]; }

protected:
    std::vector<Count> _cumulativeNumFound;
};

////////////////////////////////////////////////////////////////////////////////
template <class RangeT>
StaticDictionary::StaticDictionary(Ord maxOrd, const RangeT& countMap) {
    _cumulativeNumFound.resize(maxOrd);
    auto currOrd = Ord{0};
    auto currCumulativeNumFound = Count{0};
    for (auto& [ord, count]: countMap) {
        for (; currOrd < ord; ++currOrd) {
            _cumulativeNumFound[currOrd] = currCumulativeNumFound;
        }
        currCumulativeNumFound += count;
    }
    for (; currOrd < maxOrd; ++currOrd) {
        _cumulativeNumFound[currOrd] = currCumulativeNumFound;
    }
}

}  // namespace ael::dict

#endif // STATIC_DICTIONARY_HPP
