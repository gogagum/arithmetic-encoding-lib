#ifndef ADAPTIVE_A_DICTIONARY_HPP
#define ADAPTIVE_A_DICTIONARY_HPP

#include <cstdint>

#include "impl/a_d_dictionary_base.hpp"
#include "impl/contectual_dictionary_base_improved.hpp"
#include "impl/contextual_dictionary_base.hpp"
#include "impl/word_probability_stats.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveADictionary class
///
class AdaptiveADictionary : protected impl::ADDictionaryBase {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = WordProbabilityStats<Count>;
  constexpr const static std::uint16_t countNumBits = 62;

 public:
  /**
   * Adaptive <<A>> dictionary constructor.
   * @param maxOrd - maximal order.
   */
  explicit AdaptiveADictionary(Ord maxOrd);

  /**
   * @brief getWordOrd - get word order index by cumulative count.
   * @param cumulativeNumFound search key.
   * @return word with exact cumulative number found.
   */
  [[nodiscard]] Ord getWordOrd(Count cumulativeCnt) const;

  /**
   * @brief getWordProbabilityStats - get probability stats and update.
   * @param word - order of a word.
   * @return [low, high, total]
   */
  [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

  /**
   * @brief totalWordsCount - get total words count estimation.
   * @return total words count estimation
   */
  [[nodiscard]] Count getTotalWordsCnt() const;

 protected:
  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const;

  [[nodiscard]] Count getWordCnt_(Ord ord) const;

  [[nodiscard]] ProbabilityStats getProbabilityStats_(Ord ord) const;

 private:
  friend class impl::ContextualDictionaryStatsBase<AdaptiveADictionary>;
  friend class impl::ContextualDictionaryBase<AdaptiveADictionary>;
  friend class impl::ContextualDictionaryBaseImproved<AdaptiveADictionary>;
};

}  // namespace ael::dict

#endif  // ADAPTIVE_A_DICTIONARY_HPP
