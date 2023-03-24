#ifndef ADAPTIVE_DICTIONARY_HPP
#define ADAPTIVE_DICTIONARY_HPP

#include "word_probability_stats.hpp"
#include "impl/adaptive_dictionary_base.hpp"

#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionary class
///
class AdaptiveDictionary : public impl::AdaptiveDictionaryBase<std::uint64_t> {
public:

    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
    constexpr const static std::uint16_t countNumBits = 62; 

public:

    /**
     * @brief AdaptiveDictionary constructor.
     * @param maxOrd - maximal order.
     * @param ratio - old to new symbol probability ratio.
     */
    AdaptiveDictionary(Ord maxOrd, std::uint64_t ratio);

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getProbabilityStats reads probability statistics, updates them,
     * and returns read.
     * @param word
     * @return probability estimation for a word.
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief getTotalWordsCount get total number of words according to model.
     * @return
     */
    [[nodiscard]] Count getTotalWordsCnt() const { return this->_totalWordsCnt; }

private:

    Count _getLowerCumulativeCnt(Ord ord) const;

    void _updateWordCnt(Ord ord);

private:
    Count _ratio;
    Ord _maxOrder;
};

}  // namecpace ael::dict

#endif // ADAPTIVE_DICTIONARY_HPP
