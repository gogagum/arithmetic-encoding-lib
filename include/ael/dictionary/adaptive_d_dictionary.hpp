#ifndef AEL_DICT_ADAPTIVE_D_DICTIONARY_HPP
#define AEL_DICT_ADAPTIVE_D_DICTIONARY_HPP

#include <ael/impl/dictionary/a_d_dictionary_base.hpp>
#include <ael/impl/dictionary/contectual_dictionary_base_improved.hpp>
#include <ael/impl/dictionary/contextual_dictionary_base.hpp>
#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDDictionary class
///
class AdaptiveDDictionary : protected ael::impl::dict::ADDictionaryBase {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;
  constexpr const static std::uint16_t countNumBits = 62;

 public:
  AdaptiveDDictionary() = delete;

  /**
   * Adaptive <<D>> dictionary constructor.
   * @param maxOrd - maximal order.
   */
  explicit AdaptiveDDictionary(Ord maxOrd);

  /**
   * @brief getWord - get word by cumulative num found.
   * @param cumulativeCnt - search key.
   * @return word with exact cumulative number found.
   */
  [[nodiscard]] Ord getWordOrd(Count cumulativeCnt) const;

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
  friend class ael::impl::dict::ContextualDictionaryStatsBase<
      AdaptiveDDictionary>;
  friend class ael::impl::dict::ContextualDictionaryBase<AdaptiveDDictionary>;
  friend class ael::impl::dict::ContextualDictionaryBaseImproved<
      AdaptiveDDictionary>;
};

}  // namespace ael::dict

#endif  // AEL_DICT_ADAPTIVE_D_DICTIONARY_HPP
