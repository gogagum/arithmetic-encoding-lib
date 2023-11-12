#ifndef AEL_DICT_ADAPTIVE_A_DICTIONARY_HPP
#define AEL_DICT_ADAPTIVE_A_DICTIONARY_HPP

#include <ael/impl/dictionary/a_d_dictionary_base.hpp>
#include <ael/impl/dictionary/contextual_dictionary_base_improved.hpp>
#include <ael/impl/dictionary/contextual_dictionary_base.hpp>
#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveADictionary class
///
class AdaptiveADictionary : protected ael::impl::dict::ADDictionaryBase {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;
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
  friend class ael::impl::dict::ContextualDictionaryStatsBase<
      AdaptiveADictionary>;
  friend class ael::impl::dict::ContextualDictionaryBase<AdaptiveADictionary>;
  friend class ael::impl::dict::ContextualDictionaryBaseImproved<
      AdaptiveADictionary>;
};

}  // namespace ael::dict

#endif  // AEL_DICT_ADAPTIVE_A_DICTIONARY_HPP
