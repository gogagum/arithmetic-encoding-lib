#ifndef CONTEXTUAL_DICTIONARY_BASE_IMPROVED_HPP
#define CONTEXTUAL_DICTIONARY_BASE_IMPROVED_HPP

#include <cstdint>
#include <optional>

#include "contextual_dictionary_stats_base.hpp"
#include "word_probability_stats.hpp"

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ContextualDictionaryBaseImproved<InternalDictT> class.
///
template <class InternalDictT>
class ContextualDictionaryBaseImproved
    : protected ContextualDictionaryStatsBase<InternalDictT> {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = WordProbabilityStats<Count>;
  constexpr const static std::uint16_t countNumBits = 62;

 public:
  using ContextualDictionaryStatsBase<
      InternalDictT>::ContextualDictionaryStatsBase;

  /**
   * @brief getWord
   * @param cumulativeNumFound
   * @return
   */
  [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

  /**
   * @brief getWordProbabilityStats
   * @param word
   * @return
   */
  [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

  /**
   * @brief getTotalWordsCount get total number of words according to model.
   * @return totalWordsCount according to dictionary model.
   */
  [[nodiscard]] Count getTotalWordsCnt() const;
};

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
auto ContextualDictionaryBaseImproved<InternalDictT>::getWordOrd(
    Count cumulativeNumFound) const -> Ord {
  for (auto ctxLength = this->getCurrCtxLength_(); ctxLength != 0;
       --ctxLength) {
    const auto searchCtx = this->getSearchCtx_(ctxLength);
    if (this->getContextualTotalWordCnt_(searchCtx) >= ctxLength) {
      return this->getContextualWordOrd_(searchCtx, cumulativeNumFound);
    }
  }
  return InternalDictT::getWordOrd(cumulativeNumFound);
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
auto ContextualDictionaryBaseImproved<InternalDictT>::getProbabilityStats(
    Ord ord) -> ProbabilityStats {
  std::optional<ProbabilityStats> ret{};
  for (auto ctxLength = this->getCurrCtxLength_(); ctxLength != 0;
       --ctxLength) {
    const auto searchCtx = this->getSearchCtx_(ctxLength);
    if (this->getContextualTotalWordCnt_(searchCtx) >= ctxLength) {
      ret = ret.value_or(this->getContextualProbStats_(searchCtx, ord));
    }
    this->updateContextualDictionary_(searchCtx, ord);
  }
  ret = ret.value_or(InternalDictT::getProbabilityStats_(ord));
  this->updateWordCnt_(ord, 1);
  this->updateCtx_(ord);
  return ret.value();
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
auto ContextualDictionaryBaseImproved<InternalDictT>::getTotalWordsCnt() const
    -> Count {
  for (auto ctxLength = this->getCurrCtxLength_(); ctxLength != 0;
       --ctxLength) {
    const auto searchCtx = this->getSearchCtx_(ctxLength);
    if (this->getContextualTotalWordCnt_(searchCtx) >= ctxLength) {
      return this->getContextualTotalWordCnt_(searchCtx);
    }
  }
  return InternalDictT::getTotalWordsCnt();
}

}  // namespace ael::dict::impl

#endif  // CONTEXTUAL_DICTIONARY_BASE_IMPROVED_HPP
