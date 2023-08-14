#ifndef AEL_DICT_PPMA_DICTIONARY_HPP
#define AEL_DICT_PPMA_DICTIONARY_HPP

#include <ael/impl/dictionary/cumulative_count.hpp>
#include <ael/impl/dictionary/cumulative_unique_count.hpp>
#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <deque>
#include <unordered_map>
#include <ael/impl/dictionary/ppm_a_d_dictionary_base.hpp>

namespace ael::dict {

namespace bm = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMADictionary - ppma probability model.
///
class PPMADictionary : ael::impl::dict::PPMADDictionaryBase {
 protected:
  using Base_ = ael::impl::dict::PPMADDictionaryBase;

 public:
  using Ord = Base_::Ord;
  using Count = bm::uint256_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;
  constexpr const static std::uint16_t countNumBits = 240;

  ////////////////////////////////////////////////////////////////////////////
  /// \brief The PPMADictionary::ConstructInfo class.
  ///
  struct ConstructInfo {
    Ord maxOrd{2};
    std::size_t ctxLength{0};
  };

 private:
  constexpr static std::uint16_t _maxCtxLength = 16;
  constexpr static std::uint16_t _maxSeqLenLog2 = 40;

 public:
  /**
   * @brief PPMA dictionary constructor.
   * 
   * @param constructInfo - maximal order and context length.
   */
  explicit PPMADictionary(ConstructInfo constructInfo);

  /**
   * @brief getWordOrd - get word order index by cumulative count.
   * 
   * @param cumulativeNumFound search key.
   * @return word with exact cumulative number found.
   */
  [[nodiscard]] Ord getWordOrd(const Count& cumulativeNumFound) const;

  /**
   * @brief getWordProbabilityStats - get probability stats and update.
   * 
   * @param ord - order of a word.
   * @return [low, high, total]
   */
  [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

  /**
   * @brief getTotalWordsCount - get total words count estimation.
   * 
   * @return total words count estimation
   */
  [[nodiscard]] Count getTotalWordsCnt() const;

 private:
  using SearchCtx_ = boost::container::static_vector<Ord, _maxCtxLength>;
  using SearchCtxHash_ = boost::hash<SearchCtx_>;
  using CtxCountMapping_ =
      std::unordered_map<SearchCtx_, ael::impl::dict::CumulativeCount,
                         SearchCtxHash_>;

 private:
  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const;

  [[nodiscard]] ProbabilityStats getProbabilityStats_(Ord ord) const;

  void updateWordCnt_(Ord ord, std::int64_t cnt);

  [[nodiscard]] SearchCtx_ getSearchCtxEmptySkipped_() const;

 private:
  ael::impl::dict::CumulativeCount zeroCtxCnt_;
  ael::impl::dict::CumulativeUniqueCount zeroCtxUniqueCnt_;
  std::deque<Ord> ctx_;
  CtxCountMapping_ ctxInfo_;
  const std::size_t ctxLength_;
};

}  // namespace ael::dict

#endif  // AEL_DICT_PPMA_DICTIONARY_HPP
