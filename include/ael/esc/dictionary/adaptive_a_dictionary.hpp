#ifndef AEL_ESC_DICT_ADAPTIVE_A_DICTIONARY_HPP
#define AEL_ESC_DICT_ADAPTIVE_A_DICTIONARY_HPP

#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <ael/impl/esc/dictionary/a_d_dictionary_base.hpp>
#include <boost/container/static_vector.hpp>
#include <cstdint>

namespace ael::esc::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveADictionary class
///
class AdaptiveADictionary : public ael::impl::esc::dict::ADDictionaryBase {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;
  using StatsSeq = boost::container::static_vector<ProbabilityStats, 2>;
  constexpr const static std::uint16_t countNumBits = 62;

 public:
  AdaptiveADictionary() = delete;

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
   * @brief get probability stats and update.
   * @param ord - order of a word.
   * @return a sequence of stats of the following structure: [low, high, total]
   */
  [[nodiscard]] StatsSeq getProbabilityStats(Ord ord);

  /**
   * @brief get probability stats for decoding.
   *
   * @param ord index of a word.
   * @return [low, high, total]
   */
  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats(Ord ord);

  /**
   * @brief totalWordsCount - get total words count estimation.
   * @return total words count estimation
   */
  [[nodiscard]] Count getTotalWordsCnt() const;

 protected:
  [[nodiscard]] Ord getWordOrdAfterEsc_(Count cumulativeCnt) const;

  [[nodiscard]] StatsSeq getProbabilityStats_(Ord ord) const;

  [[nodiscard]] StatsSeq getProbabilityStatsForNewWord_(Ord ord) const;

  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats_(Ord ord);

 private:
  bool escJustDecoded_{false};
};

}  // namespace ael::esc::dict

#endif  // AEL_ESC_DICT_ADAPTIVE_A_DICTIONARY_HPP
