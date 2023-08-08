#ifndef PPMD_DICTIONARY_HPP
#define PPMD_DICTIONARY_HPP

#include <ael/impl/dictionary/cumulative_count.hpp>
#include <ael/impl/dictionary/cumulative_unique_count.hpp>
#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <unordered_map>
#include <ael/impl/dictionary/ppm_a_d_dictionary_base.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMDDictionary - ppmd probability model.
///
class PPMDDictionary : ael::impl::dict::PPMADDictionaryBase {
 protected:
  using Base_ = ael::impl::dict::PPMADDictionaryBase;
  using CumulativeCount_ = ael::impl::dict::CumulativeCount;
  using CumulativeUniqueCount_ = ael::impl::dict::CumulativeUniqueCount;

 public:
  using Ord = Base_::Ord;
  using Count = boost::multiprecision::uint256_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;
  constexpr const static std::uint16_t countNumBits = 240;

  ////////////////////////////////////////////////////////////////////////////
  /// \brief The PPMDDictionary::ConstructInfo class.
  ///
  struct ConstructInfo {
    Ord maxOrd{2};
    std::size_t ctxLength{0};
  };

 private:
  constexpr const static std::uint16_t _maxCtxLength = 16;
  constexpr const static std::uint16_t _maxSeqLenLog2 = 40;

 public:
  /**
   * PPMD dictionary constructor.
   * @param maxOrd - maximal order and context length.
   */
  explicit PPMDDictionary(ConstructInfo constructInfo);

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
  using SearchCtx_ = boost::container::static_vector<Ord, _maxCtxLength>;
  using SearchCtxHash_ = boost::hash<SearchCtx_>;
  struct CtxCell_ {
    explicit CtxCell_(Ord maxOrd) : cnt(maxOrd), uniqueCnt(maxOrd) {}
    CumulativeCount_ cnt;
    CumulativeUniqueCount_ uniqueCnt;
  };
  using CtxCountMapping_ =
      std::unordered_map<SearchCtx_, CtxCell_, SearchCtxHash_>;

 private:
  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const;

  [[nodiscard]] ProbabilityStats getProbabilityStats_(Ord ord) const;

  void updateWordCnt_(Ord ord, std::int64_t cnt);

  [[nodiscard]] SearchCtx_ getSearchCtxEmptySkipped_() const;

 private:
  CtxCell_ zeroCtxCell_;
  std::deque<Ord> ctx_;
  CtxCountMapping_ ctxInfo_;
  const std::size_t ctxLength_;
};

}  // namespace ael::dict

#endif  // PPMD_DICTIONARY_HPP
