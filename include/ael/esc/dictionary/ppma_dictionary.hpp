#ifndef ESC_PPMA_DICTIONARY_HPP
#define ESC_PPMA_DICTIONARY_HPP

#include <ael/impl/dictionary/cumulative_count.hpp>
#include <ael/impl/dictionary/cumulative_unique_count.hpp>
#include <ael/impl/esc/dictionary/ppm_a_d_dictionary_base.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>
#include <cstdint>
#include <deque>
#include <unordered_map>

namespace ael::esc::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMADictionary - PPMA escape version dictionary class.
///
class PPMADictionary : public ael::impl::esc::dict::PPMADDictionaryBase {
 private:
  using Base_ = ael::impl::esc::dict::PPMADDictionaryBase;
  using CumulativeCount_ = ael::impl::dict::CumulativeCount;
  using CumulativeUniqueCount_ = ael::impl::dict::CumulativeUniqueCount;
  constexpr static std::uint16_t _maxCtxLength = Base_::maxCtxLength_;
  constexpr static std::uint16_t _maxSeqLenLog2 = 40;

 public:
  using Ord = Base_::Ord;
  using Count = Base_::Count;
  using ProbabilityStats = Base_::ProbabilityStats;
  using StatsSeq =
      boost::container::static_vector<ProbabilityStats, _maxCtxLength>;
  constexpr static std::uint16_t countNumBits = 62;

  struct ConstructInfo {
    Ord maxOrd{2};
    std::size_t ctxLength{0};
  };

 public:
  /**
   * @brief PPMA dictionary with esc symbols constructor.
   *
   * @param constructInfo - maximal order and context length.
   */
  explicit PPMADictionary(ConstructInfo constructInfo);

  /**
   * @brief get word order (index) by cumulative count.
   *
   * @param cumulativeCnt search key.
   * @return word with exact cumulative count estimation.
   */
  [[nodiscard]] Ord getWordOrd(Count cumulativeCnt) const;

  /**
   * @brief get probability statistics for encoding and update.
   *
   * @param ord order (index) of a word.
   * @return a sequence of statistics, each of the following structure:
   * [low, high, total]
   */
  [[nodiscard]] StatsSeq getProbabilityStats(Ord ord);

  /**
   * @brief get probability statistics for decoding.
   *
   * @param ord order (index) of a word.
   * @return [low, high, total]
   */
  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats(Ord ord);

  /**
   * @brief get total word count according to model.
   *
   * @return total words count estimation.
   */
  [[nodiscard]] Count getTotalWordsCnt() const;

 protected:
  using SearchCtx_ = Base_::SearchCtx_;

 protected:
  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats_(Ord ord);

  [[nodiscard]] ProbabilityStats getDecodeProbabilityStatsForNewWord_(
      Ord ord) const;

  void updateWordCnt_(Ord ord, std::int64_t cntChange);

  [[nodiscard]] Ord getWordOrdForNewWord_(Count cumulativeCnt) const;

  void skipNewCtxs_(SearchCtx_& currCtx) const;  // TODO(gogagum): move to base

  [[nodiscard]] ProbabilityStats getZeroCtxEscStats_() const;

 private:
  using SearchCtxHash_ = boost::hash<SearchCtx_>;
  using CtxCountMapping_ =
      std::unordered_map<SearchCtx_, CumulativeCount_, SearchCtxHash_>;

 private:
  CumulativeCount_ zeroCtxCnt_;
  CumulativeUniqueCount_ zeroCtxUniqueCnt_;
  CtxCountMapping_ ctxInfo_;
  const std::size_t ctxLength_;
};

}  // namespace ael::esc::dict

#endif
