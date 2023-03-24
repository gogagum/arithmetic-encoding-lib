#ifndef DECREASING_ON_UPDATE_DICTIONARY_HPP
#define DECREASING_ON_UPDATE_DICTIONARY_HPP

#include "word_probability_stats.hpp"
#include "impl/adaptive_dictionary_base.hpp"

#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecreasingOnUpdateDictionary class
///
class DecreasingOnUpdateDictionary 
    : public impl::AdaptiveDictionaryBase<std::uint64_t> {
public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
    constexpr const static std::uint16_t countNumBits = 62; 
public:    

    /**
     * @brief DecreasingOnUpdateDictionary constructor from counts mapping
     * @param probRng - counts mapping.
     */
    template <std::ranges::input_range RangeT>
    DecreasingOnUpdateDictionary(Ord maxOrd, const RangeT& probRng);

    /**
     * @brief DecreasingOnUpdateDictionary - generate uniform with `count for
     * each word.
     * @param count
     */
    DecreasingOnUpdateDictionary(Ord maxOrd, Count count);

    /**
     * @brief getWord - word by cumulatove count.
     * @param cumulativeNumFound - count to search for.
     * @return found word.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats - get stats for a word.
     * @param word - word to get stats for.
     * @return [low, high, total]
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief getTotalWordsCount - get total words count.
     * @return total words count in a dictionary.
     */
    [[nodiscard]] Count
    getTotalWordsCnt() const { return this->_totalWordsCnt; }

protected:

    Count _getLowerCumulativeCnt(Ord ord) const;

    void _updateWordCnt(Ord ord, Count cnt);

    ProbabilityStats _getProbabilityStats(Ord ord) const;

protected:
    const Ord _maxOrd;
};

////////////////////////////////////////////////////////////////////////////////
template <std::ranges::input_range RangeT>
DecreasingOnUpdateDictionary::DecreasingOnUpdateDictionary(
        Ord maxOrd,
        const RangeT& countRng
        ) : impl::AdaptiveDictionaryBase<Count>(maxOrd, 0),
            _maxOrd(maxOrd) {
    for (const auto& [ord, count] : countRng) {
        this->_wordCnts[ord] = count;
        this->_cumulativeWordCounts.update(ord, _maxOrd, count);
        this->_totalWordsCnt += count;
    }
}

}  // namespace ael::dict

#endif // DECREASING_TOTAL_COUNT_DICTIONARY_HPP
