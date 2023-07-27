#ifndef ESC_ADAPTIVE_D_DICTIONARY_HPP
#define ESC_ADAPTIVE_D_DICTIONARY_HPP

#include <boost/container/static_vector.hpp>
#include <cstdint>

#include <ael/dictionary/impl/a_d_dictionary_base.hpp>
#include <ael/dictionary/impl/word_probability_stats.hpp>

namespace ael::esc::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDDictionary class
///
class AdaptiveDDictionary : protected ael::dict::impl::ADDictionaryBase {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::dict::WordProbabilityStats<Count>;
  using StatsSeq = boost::container::static_vector<ProbabilityStats, 2>;
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
  [[nodiscard]] Ord getWordOrd(Count cumulativeCnt) const;

  /**
   * @brief getWordOrd - get word order index by cumulative count.
   * @param cumulativeNumFound search key.
   * @return word with exact cumulative number found.
   */
  [[nodiscard]] StatsSeq getProbabilityStats(Ord ord);

  /**
   * @brief get probability stats for decoding.
   * 
   * @param ord - order of a word.
   * @return [low, high, total]
   */
  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats(Ord ord);

  /**
   * @brief totalWordsCount
   * @return
   */
  [[nodiscard]] Count getTotalWordsCnt() const;

  /**
   * @brief check if ord is esc-symbol.
   * 
   * @param ord to check.
   * @return true if word is esc symbol.
   * @return false if word is not an esc symbol.
   */
  [[nodiscard]] bool isEsc(Ord ord) const;

 protected:
  [[nodiscard]] Ord getWordOrdAfterEsc_(Count cumulativeCnt) const;

  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const;

  [[nodiscard]] Count getWordCnt_(Ord ord) const;

  [[nodiscard]] StatsSeq getProbabilityStats_(Ord ord) const;

  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats_(Ord ord) const;

  [[nodiscard]] StatsSeq getProbabilityStatsForNewWord_(Ord ord) const;

 private:
  mutable bool escJustDecoded_{false};
};

}  // namespace ael::esc::dict

#endif  // ESC_ADAPTIVE_D_DICTIONARY_HPP
