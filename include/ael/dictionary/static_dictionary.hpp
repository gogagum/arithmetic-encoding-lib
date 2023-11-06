#ifndef AEL_DICT_STATIC_DICTIONARY_HPP
#define AEL_DICT_STATIC_DICTIONARY_HPP

#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <cstdint>
#include <map>
#include <ranges>
#include <vector>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The StaticDictionary class
///
class StaticDictionary {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;
  constexpr static std::uint16_t countNumBits = 62;

  struct CountMapping {
    Ord ord;
    Count count;
  };

 public:
  /**
   * @brief fromCounts construct from simple wordsCounts
   * @param countsRng - count of each symbol ordered.
   * @return
   */
  template <std::ranges::input_range RangeT>
  explicit StaticDictionary(Ord maxOrd, const RangeT& countsRng)
    requires std::is_same_v<std::ranges::range_value_t<RangeT>, CountMapping>;

  explicit StaticDictionary(Ord maxOrd, const std::map<Ord, Count>& countsRng);

  /**
   * @brief getWord - get word by cumulative num found.
   * @param cumulativeNumFound - search key.
   * @return word with exact cumulative number found.
   */
  [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

  /**
   * @brief getProbabilityStats - get lower cumulative number of words,
   * higher cumulative number of words and total count.
   * @param word - word to get info for.
   * @return statistics.
   */
  [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

  /**
   * @brief totalWordsCount
   * @return
   */
  [[nodiscard]] Ord getTotalWordsCnt() const {
    return *cumulativeNumFound_.rbegin();
  }

 private:
  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const;

  [[nodiscard]] Count getHigherCumulativeCnt_(Ord ord) const {
    return cumulativeNumFound_[ord];
  }

 private:
  template <class Rng>
  std::map<Ord, Count> countCntMapping_(const Rng& countsRng);

 private:
  std::vector<Count> cumulativeNumFound_{};
};

////////////////////////////////////////////////////////////////////////////////
template <std::ranges::input_range RangeT>
StaticDictionary::StaticDictionary(Ord maxOrd, const RangeT& countsRng)
  requires std::is_same_v<std::ranges::range_value_t<RangeT>, CountMapping>
    : StaticDictionary(maxOrd, countCntMapping_(countsRng)) {
}

////////////////////////////////////////////////////////////////////////////////
template <class Rng>
auto StaticDictionary::countCntMapping_(const Rng& countsRng)
    -> std::map<Ord, Count> {
  std::map<Ord, Count> countMapping;
  for (const auto& [ord, count] : countsRng) {
    countMapping[ord] += count;
  }
  return countMapping;
}

}  // namespace ael::dict

#endif  // AEL_DICT_STATIC_DICTIONARY_HPP
