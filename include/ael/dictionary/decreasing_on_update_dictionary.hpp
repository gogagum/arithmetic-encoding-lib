#ifndef AEL_DICT_DECREASING_ON_UPDATE_DICTIONARY_HPP
#define AEL_DICT_DECREASING_ON_UPDATE_DICTIONARY_HPP

#include <ael/impl/dictionary/adaptive_dictionary_base.hpp>
#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecreasingOnUpdateDictionary class
///
class DecreasingOnUpdateDictionary
    : public ael::impl::dict::AdaptiveDictionaryBase<std::uint64_t> {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;
  constexpr const static std::uint16_t countNumBits = 62;

 public:
  /**
   * @brief DecreasingOnUpdateDictionary constructor from counts mapping
   * @param probRng - counts mapping.
   */
  template <std::ranges::input_range RangeT>
  DecreasingOnUpdateDictionary(Ord maxOrd, const RangeT& countRng);

  /**
   * @brief DecreasingOnUpdateDictionary - generate uniform with `count for
   * each word.
   * @param count
   */
  DecreasingOnUpdateDictionary(Ord maxOrd, Count count);

  /**
   * @brief getWord - word by cumulatove count.
   * @param cumulativeCnt - count to search for.
   * @return found word.
   */
  [[nodiscard]] Ord getWordOrd(Count cumulativeCnt) const;

  /**
   * @brief getWordProbabilityStats - get stats for a word.
   * @param word - word to get stats for.
   * @return [low, high, total]
   */
  [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

  /**
   * @brief getTotalWordsCount - get total words count.
   * @return total words count in a dictionary.
   */
  [[nodiscard]] Count getTotalWordsCnt() const {
    return getRealTotalWordsCnt_();
  }

 protected:
  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const {
    return (ord == Ord{0}) ? Count{0} : getRealCumulativeCnt_(ord - 1);
  }

  void updateWordCnt_(Ord ord, Count cnt);

  [[nodiscard]] ProbabilityStats getProbabilityStats_(Ord ord) const;

 private:
  const Ord maxOrd_;
};

////////////////////////////////////////////////////////////////////////////////
template <std::ranges::input_range RangeT>
DecreasingOnUpdateDictionary::DecreasingOnUpdateDictionary(
    Ord maxOrd, const RangeT& countRng)
    : ael::impl::dict::AdaptiveDictionaryBase<Count>(maxOrd, 0),
      maxOrd_(maxOrd) {
  for (const auto& [ord, count] : countRng) {
    changeRealWordCnt_(ord, count);
    changeRealCumulativeWordCnt_(ord, count);
    changeRealTotalWordsCnt_(count);
  }
}

}  // namespace ael::dict

#endif  // AEL_DICT_DECREASING_TOTAL_COUNT_DICTIONARY_HPP
