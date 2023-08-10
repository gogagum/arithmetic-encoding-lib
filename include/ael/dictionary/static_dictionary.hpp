#ifndef STATIC_DICTIONARY_HPP
#define STATIC_DICTIONARY_HPP

#include <cstdint>
#include <vector>
#include <ranges>

#include <ael/impl/dictionary/word_probability_stats.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The StaticDictionary class
///
class StaticDictionary {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;

 public:
  /**
   * @brief fromCounts construct from simple wordsCounts
   * @param countsRng - count of each symbol ordered.
   * @return
   */
  template <std::ranges::input_range RangeT>
  explicit StaticDictionary(Ord maxOrd, const RangeT& countsRng);

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
  [[nodiscard]] Ord getTotalWordsCount() const {
    return *cumulativeNumFound_.rbegin();
  }

 private:
  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const;

  [[nodiscard]] Count getHigherCumulativeCnt_(Ord ord) const {
    return cumulativeNumFound_[ord];
  }

 private:
  std::vector<Count> cumulativeNumFound_{};
};

////////////////////////////////////////////////////////////////////////////////
template <std::ranges::input_range RangeT>
StaticDictionary::StaticDictionary(Ord maxOrd, const RangeT& countsRng) {
  cumulativeNumFound_.resize(maxOrd);
  auto currOrd = Ord{0};
  auto currCumulativeNumFound = Count{0};
  for (auto& [ord, count] : countsRng) {
    for (; currOrd < ord; ++currOrd) {
      cumulativeNumFound_[currOrd] = currCumulativeNumFound;
    }
    currCumulativeNumFound += count;
  }
  for (; currOrd < maxOrd; ++currOrd) {
    cumulativeNumFound_[currOrd] = currCumulativeNumFound;
  }
}

}  // namespace ael::dict

#endif  // STATIC_DICTIONARY_HPP
