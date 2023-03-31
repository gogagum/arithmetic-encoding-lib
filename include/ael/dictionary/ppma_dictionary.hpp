#ifndef PPMA_DICTIONARY_HPP
#define PPMA_DICTIONARY_HPP

#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <deque>
#include <unordered_map>

#include "ael/dictionary/impl/cumulative_count.hpp"
#include "ael/dictionary/impl/cumulative_unique_count.hpp"
#include "impl/word_probability_stats.hpp"

namespace ael::dict {

namespace bm = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMADictionary - ppma probability model.
///
class PPMADictionary {
 public:
  using Ord = std::uint64_t;
  using Count = bm::uint256_t;
  using ProbabilityStats = WordProbabilityStats<Count>;
  constexpr const static std::uint16_t countNumBits = 240;

  ////////////////////////////////////////////////////////////////////////////
  /// \brief The PPMADictionary::ConstructInfo class.
  ///
  struct ConstructInfo {
    Ord maxOrd;
    std::size_t ctxLength;
  };

 private:
  constexpr const static std::uint16_t _maxSeqLenLog2 = 40;

 public:
  /**
   * PPMA dictionary constructor.
   * @param constructInfo - maximal order and context length.
   */
  explicit PPMADictionary(ConstructInfo constructInfo);

  /**
   * @brief getWordOrd - get word order index by cumulative count.
   * @param cumulativeNumFound search key.
   * @return word with exact cumulative number found.
   */
  [[nodiscard]] Ord getWordOrd(const Count& cumulativeNumFound) const;

  /**
   * @brief getWordProbabilityStats - get probability stats and update.
   * @param ord - order of a word.
   * @return [low, high, total]
   */
  [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

  /**
   * @brief getTotalWordsCount - get total words count estimation.
   * @return total words count estimation
   */
  [[nodiscard]] Count getTotalWordsCnt() const;

 private:
  using SearchCtx_ = boost::container::static_vector<Ord, 16>;
  using SearchCtxHash_ = boost::hash<SearchCtx_>;
  using CtxCountMapping_ =
      std::unordered_map<SearchCtx_, impl::CumulativeCount, SearchCtxHash_>;

 private:
  Count getLowerCumulativeCnt_(Ord ord) const;

  ProbabilityStats getProbabilityStats_(Ord ord) const;

  void updateWordCnt_(Ord ord, impl::CumulativeCount::Count cnt);

  SearchCtx_ getSearchCtxEmptySkipped_() const;

 private:
  std::size_t maxOrd_;
  impl::CumulativeCount zeroCtxCnt_;
  impl::CumulativeUniqueCount zeroCtxUniqueCnt_;
  std::deque<Ord> ctx_;
  CtxCountMapping_ ctxInfo_;
  const std::size_t ctxLength_;
};

}  // namespace ael::dict

#endif  // PPMA_DICTIONARY_HPP
