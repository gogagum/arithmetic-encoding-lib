#ifndef AEL_ESC_DICT_PPMD_DICTIONARY_HPP
#define AEL_ESC_DICT_PPMD_DICTIONARY_HPP

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
class PPMDDictionary
    : public ael::impl::esc::dict::PPMADDictionaryBase<PPMDDictionary> {
 private:
  using Base_ = ael::impl::esc::dict::PPMADDictionaryBase<PPMDDictionary>;
  using CumulativeCount_ = impl::dict::CumulativeCount;
  using CumulativeUniqueCount_ = impl::dict::CumulativeUniqueCount;
  constexpr static auto maxCtxLength_ = std::uint16_t{16};
  constexpr static std::uint16_t maxSeqLenLog2_ = 40;

 public:
  using Ord = Base_::Ord;
  using Count = Base_::Count;
  using ProbabilityStats = Base_::ProbabilityStats;
  using StatsSeq =
      boost::container::static_vector<ProbabilityStats, maxCtxLength_ + 2>;
  constexpr static std::uint16_t countNumBits = 62;

  struct ConstructInfo {
    Ord maxOrd{2};
    std::size_t ctxLength{0};
  };

 public:
  PPMDDictionary() = delete;

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
  using SearchCtx_ = Base_::SearchCtx_;

  struct CtxCell_ {
    explicit CtxCell_(Ord maxOrd) : cnt(maxOrd), uniqueCnt(maxOrd) {
    }

    CumulativeCount_ cnt;
    CumulativeUniqueCount_ uniqueCnt;
  };

 protected:
  [[nodiscard]] ProbabilityStats getDecodeProbabilityStats_(Ord ord);

  [[nodiscard]] ProbabilityStats getDecodeProbabilityStatsForNewWord_(
      Ord ord) const;

  void updateWordCnt_(Ord ord, std::int64_t cntChange);

  [[nodiscard]] Ord getWordOrdForNewWord_(Count cumulativeCnt) const;

  [[nodiscard]] ProbabilityStats getZeroCtxEscStats_() const;

  [[nodiscard]] const CtxCell_& getCurrCtxCell_(SearchCtx_&& currCtx) const;

 private:
  using SearchCtxHash_ = boost::hash<SearchCtx_>;
  using CtxCountMapping_ =
      std::unordered_map<SearchCtx_, CtxCell_, SearchCtxHash_>;

 private:
  CtxCell_ zeroCtxCell_;
  CtxCountMapping_ ctxInfo_;

 private:
  template <class DictT, typename CountT, std::uint16_t maxCtxLength>
  friend class ael::impl::dict::CtxBase;
};

}  // namespace ael::esc::dict

#endif  // AEL_ESC_DICT_PPMD_DICTIONARY_HPP