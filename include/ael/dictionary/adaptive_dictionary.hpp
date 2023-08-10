#ifndef ADAPTIVE_DICTIONARY_HPP
#define ADAPTIVE_DICTIONARY_HPP

#include <ael/impl/dictionary/adaptive_dictionary_base.hpp>
#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionary class
///
class AdaptiveDictionary
    : public ael::impl::dict::AdaptiveDictionaryBase<std::uint64_t> {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;
  constexpr const static std::uint16_t countNumBits = 62;

  ////////////////////////////////////////////////////////////////////////////
  /// \brief The AdaptiveDictionary::ConstructInfo class
  ///
  struct ConstructInfo {
    Ord maxOrd{2};
    std::uint64_t ratio{4};
  };

 public:
  AdaptiveDictionary() = delete;

  /**
   * @brief AdaptiveDictionary constructor.
   * @param constructInfo - maximal order and ratio.
   */
  explicit AdaptiveDictionary(ConstructInfo constructInfo);

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
  [[nodiscard]] Count getTotalWordsCnt() const {
    return maxOrder_ + ratio_ * getRealTotalWordsCnt_();
  }

 private:
  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const;

  void updateWordCnt_(Ord ord);

 private:
  Count ratio_;
  Ord maxOrder_;
};

}  // namespace ael::dict

#endif  // ADAPTIVE_DICTIONARY_HPP
