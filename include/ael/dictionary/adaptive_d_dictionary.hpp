#ifndef ADAPTIVE_D_DICTIONARY_HPP
#define ADAPTIVE_D_DICTIONARY_HPP

#include "ael/dictionary/impl/contectual_dictionary_base_improved.hpp"
#include "word_probability_stats.hpp"
#include "impl/a_d_dictionary_base.hpp"
#include "impl/contextual_dictionary_base.hpp"

#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDDictionary class
///
class AdaptiveDDictionary : protected impl::ADDictionaryBase {
public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
    constexpr const static std::uint16_t countNumBits = 62; 
public:

    /**
     * Adaptive <<D>> dictionary constructor.
     * @param maxOrd - maximal order. 
     */
    AdaptiveDDictionary(Ord maxOrd);

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief totalWordsCount
     * @return
     */
    [[nodiscard]] Count getTotalWordsCnt() const;

protected:

    Count _getLowerCumulativeCnt(Ord ord) const;

    Count _getWordCnt(Ord ord) const;

    ProbabilityStats _getProbabilityStats(Ord ord) const;

private:
    friend class impl::ContextualDictionaryStatsBase<AdaptiveDDictionary>;
    friend class impl::ContextualDictionaryBase<AdaptiveDDictionary>;
    friend class impl::ContextualDictionaryBaseImproved<AdaptiveDDictionary>;
};

}  // namespace ael::dict

#endif // ADAPTIVE_D_DICTIONARY_HPP
