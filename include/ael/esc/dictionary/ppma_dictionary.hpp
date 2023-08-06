#ifndef ESC_PPMA_DICTIONARY_HPP
#define ESC_PPMA_DICTIONARY_HPP

#include <ael/impl/dictionary/cumulative_unique_count.hpp>
#include <ael/impl/dictionary/cumulative_count.hpp>
#include <ael/impl/esc/dictionary/ppm_a_d_dictionary_base.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>
#include <cstdint>
#include <deque>
#include <unordered_map>

namespace ael::esc::dict {

class PPMADictionary : public ael::impl::esc::dict::PPMADDictionaryBase {
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
   * @brief PPMA dictionary with esc symbols.
   *
   * @param constructInfo - maximal order and context length.
   */
  explicit PPMADictionary(ConstructInfo constructInfo);

  /**
   * @brief get word order (index) by cumulative count.
   *
   * @param cumulativeNUmFound search key.
   * @return word with exact cumulative count estimation.
   */
  [[nodiscard]] Ord getWordOrd(Count cumulativeCnt) const;

  /**
   * @brief get probability stats for encoding and update.
   *
   * @param ord index of a word.
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
   * @brief get total word count according to model.
   *
   * @return total words count estimation.
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

  void updateCtx_(Ord ord);

  void skipNewCtxs_(SearchCtx_& currCtx) const;  // TODO(gogagum): move to base

  void skipCtxsByEsc_(SearchCtx_& currCtx) const {
    assert(escDecoded_ < currCtx.size() && "Checked other cases.");
    currCtx.resize(currCtx.size() - escDecoded_);
  }

  [[nodiscard]] ProbabilityStats getZeroCtxEscStats_() const;

 private:
  using SearchCtxHash_ = boost::hash<SearchCtx_>;
  using CtxCountMapping_ =
      std::unordered_map<SearchCtx_, ael::impl::dict::CumulativeCount,
                         SearchCtxHash_>;

 private:
  ael::impl::dict::CumulativeCount zeroCtxCnt_;
  ael::impl::dict::CumulativeUniqueCount zeroCtxUniqueCnt_;
  std::deque<Ord> ctx_;
  CtxCountMapping_ ctxInfo_;
  const std::size_t ctxLength_;
  mutable std::size_t escDecoded_{0};
};

}  // namespace ael::esc::dict

#endif
