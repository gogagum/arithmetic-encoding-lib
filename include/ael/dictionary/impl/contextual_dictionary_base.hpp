#ifndef CONTEXTUAL_DICTIONARY_BASE_HPP
#define CONTEXTUAL_DICTIONARY_BASE_HPP

#include <cstdint>
#include <optional>
#include <ael/dictionary/word_probability_stats.hpp>

#include "contextual_dictionary_stats_base.hpp"

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ContextualDictionaryBase<InternalDictT> class
///
template<class InternalDictT>
class ContextualDictionaryBase
    : protected ContextualDictionaryStatsBase<InternalDictT> {
public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
    constexpr const static std::uint16_t countNumBits = 62; 
public:

    using ContextualDictionaryStatsBase<InternalDictT>::ContextualDictionaryStatsBase;

    /**
     * @brief getWordOrd - ord getter. 
     * @param cumulativeNumFound - search count.
     * @return - found word order.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats - probability stats getter with update.
     * @param ord order index of a word.
     * @return probability stats.
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief getTotalWordsCount get total number of words according to model.
     * @return totalWordsCount according to dictionary model.
     */
    [[nodiscard]] Count getTotalWordsCnt() const;
};

////////////////////////////////////////////////////////////////////////////////
template<class InternalDictT>
auto ContextualDictionaryBase<InternalDictT>::getWordOrd(
        Count cumulativeNumFound) const -> Ord {
    for (auto ctxLength = this->_currCtxLength; ctxLength != 0; --ctxLength) {
        const auto searchCtx = this->_getSearchCtx(ctxLength);
        if (this->_contextProbs.contains(searchCtx)) {
            return this->_getContextualWordOrd(searchCtx, cumulativeNumFound);
        }
    }
    return InternalDictT::getWordOrd(cumulativeNumFound);
}

////////////////////////////////////////////////////////////////////////////////
template<class InternalDictT>
auto ContextualDictionaryBase<InternalDictT>::getProbabilityStats(
        Ord ord) -> ProbabilityStats {
    std::optional<ProbabilityStats> ret;
    for (auto ctxLength = this->_currCtxLength; ctxLength != 0; --ctxLength) {
        const auto searchCtx = this->_getSearchCtx(ctxLength);
        if (this->_contextProbs.contains(searchCtx)) {
            ret = ret.value_or(this->_getContextualProbStats(searchCtx, ord));
        }
        this->_updateContextualDictionary(searchCtx, ord);
    }
    ret = ret.value_or(InternalDictT::_getProbabilityStats(ord));
    this->_updateWordCnt(ord, 1);
    this->_updateCtx(ord);
    return ret.value();
}

////////////////////////////////////////////////////////////////////////////////
template<class InternalDictT>
auto ContextualDictionaryBase<InternalDictT>::getTotalWordsCnt(
        ) const -> Count {
    for (auto ctxLength = this->_currCtxLength; ctxLength != 0; --ctxLength) {
        const auto searchCtx = this->_getSearchCtx(ctxLength);
        if (this->_contextProbs.contains(searchCtx)) {
            return this->_getContextualTotalWordCnt(searchCtx);
        }
    }
    return InternalDictT::getTotalWordsCnt();
}

}  // namespace ael::dict::impl

#endif  // CONTEXTUAL_DICTIONARY_BASE_HPP
