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
  explicit PPMDDictionary(ConstructInfo constructInfo);

  [[nodiscard]] Ord getWordOrd(Count cumulativrCnt) const;

  [[nodiscard]] StatsSeq getProbabilityStats(Ord ord);

  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats(Ord ord);

  [[nodiscard]] Count getTotalWordsCnt() const;
 protected:
  using SearchCtx_ = boost::container::static_vector<Ord, _maxCtxLength>;

 protected:
  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats_(Ord ord) const;

  [[nodiscard]] ProbabilityStats getDecodeProbabilityStatsForNewWord_(
      Ord ord) const;

  void updateWordCnt_(Ord ord, std::uint64_t cntChange);

  [[nodiscard]] SearchCtx_ getSearchCtxOfLength_(std::size_t ctxLength) const;

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
};

}  // namespace ael::esc::dict

#endif  // ESC_PPMD_DICTIONARY_HPP