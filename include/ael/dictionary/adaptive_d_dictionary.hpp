#ifndef ADAPTIVE_D_DICTIONARY_HPP
#define ADAPTIVE_D_DICTIONARY_HPP

#include <cstdint>

#include "impl/a_d_dictionary_base.hpp"
#include "impl/contectual_dictionary_base_improved.hpp"
#include "impl/contextual_dictionary_base.hpp"
#include "impl/word_probability_stats.hpp"

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
  explicit AdaptiveDDictionary(Ord maxOrd);

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
  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const;

  [[nodiscard]] Count getWordCnt_(Ord ord) const;

  [[nodiscard]] ProbabilityStats getProbabilityStats_(Ord ord) const;

 private:
  friend class impl::ContextualDictionaryStatsBase<AdaptiveDDictionary>;
  friend class impl::ContextualDictionaryBase<AdaptiveDDictionary>;
  friend class impl::ContextualDictionaryBaseImproved<AdaptiveDDictionary>;
};

}  // namespace ael::dict

#endif  // ADAPTIVE_D_DICTIONARY_HPP
