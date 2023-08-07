#ifndef ESC_PPMD_DICTIONARY_HPP
#define ESC_PPMD_DICTIONARY_HPP

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
/// \brief PPMDDictionary - PPMD escape version dictionary class.
///
class PPMDDictionary : public ael::impl::esc::dict::PPMADDictionaryBase {
 private:
  constexpr static std::uint16_t _maxCtxLength = 16;
  constexpr static std::uint16_t _maxSeqLenLog2 = 40;

 public:
  using Ord = impl::esc::dict::PPMADDictionaryBase::Ord;
  using Count = impl::esc::dict::PPMADDictionaryBase::Count;
  using ProbabilityStats =
      impl::esc::dict::PPMADDictionaryBase::ProbabilityStats;
  using StatsSeq =
      boost::container::static_vector<ProbabilityStats, _maxCtxLength>;
  constexpr static std::uint16_t countNumBits = 62;

  struct ConstructInfo {
    Ord maxOrd{2};
    std::size_t ctxLength{0};
  };

 public:
  /**
   * @brief PPMD dictionary with esc sympols constructor.
   *
   * @param constructInfo - maximal order and context length.
   */
  explicit PPMDDictionary(ConstructInfo constructInfo);

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
   * @param ord ordere (index) of a word.
   * @return [low, high, total]
   */
  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats(Ord ord);

  /**
   * @brief get total word count according to the model.
   *
   * @return total count.
   */
  [[nodiscard]] Count getTotalWordsCnt() const;

 protected:
  using SearchCtx_ = boost::container::static_vector<Ord, _maxCtxLength>;

 protected:
  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats_(Ord ord);

  [[nodiscard]] ProbabilityStats getDecodeProbabilityStatsForNewWord_(
      Ord ord) const;

  void updateWordCnt_(Ord ord, std::int64_t cntChange);

  [[nodiscard]] Ord getWordOrdForNewWord_(Count cumulativeCnt) const;

  void updateCtx_(Ord ord);  // TODO(gogagum): move to base

  void skipNewCtxs_(SearchCtx_& currCtx) const;  // TODO(gogagum): move to base

  void skipCtxsByEsc_(SearchCtx_& currCtx) const {  // TODO(gogagum): move to base
    assert(getEscDecoded_() < currCtx.size() && "Checked other cases.");
    currCtx.resize(currCtx.size() - getEscDecoded_());
  }

  [[nodiscard]] ProbabilityStats getZeroCtxEscStats_() const;

 private:
  using SearchCtxHash_ = boost::hash<SearchCtx_>;
  struct CtxCell_ {
    ael::impl::dict::CumulativeCount cnt;
    ael::impl::dict::CumulativeUniqueCount uniqueCnt;
  };
  using CtxCountMapping_ =
      std::unordered_map<SearchCtx_, CtxCell_, SearchCtxHash_>;

 private:
  ael::impl::dict::CumulativeCount zeroCtxCnt_;
  ael::impl::dict::CumulativeUniqueCount zeroCtxUniqueCnt_;
  std::deque<Ord> ctx_;
  CtxCountMapping_ ctxInfo_;
  const std::size_t ctxLength_;
};

}  // namespace ael::esc::dict

#endif  // ESC_PPMD_DICTIONARY_HPP