#ifndef AEL_DICT_PPMA_DICTIONARY_HPP
#define AEL_DICT_PPMA_DICTIONARY_HPP

#include <ael/impl/dictionary/cumulative_count.hpp>
#include <ael/impl/dictionary/cumulative_unique_count.hpp>
#include <ael/impl/dictionary/ppm_a_d_dictionary_base.hpp>
#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <deque>
#include <unordered_map>

namespace ael::dict {

constexpr auto ppmaMaxCtxLength = std::uint16_t{16};

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMADictionary - ppma probability model.
///
class PPMADictionary : ael::impl::dict::PPMADDictionaryBase<ppmaMaxCtxLength> {
 protected:
  using Base_ = ael::impl::dict::PPMADDictionaryBase<ppmaMaxCtxLength>;

 public:
  using Ord = Base_::Ord;
  using Count = boost::multiprecision::uint256_t;
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
  constexpr static std::uint16_t maxSeqLenLog2_ = 40;

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
  using SearchCtx_ = Base_::SearchCtx_;
  using SearchCtxHash_ = boost::hash<SearchCtx_>;
  using CtxCountMapping_ =
      std::unordered_map<SearchCtx_, ael::impl::dict::CumulativeCount,
                         SearchCtxHash_>;

 private:
  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const;

  [[nodiscard]] ProbabilityStats getProbabilityStats_(Ord ord) const;

  void updateWordCnt_(Ord ord, std::int64_t cnt);

 private:
  ael::impl::dict::CumulativeCount zeroCtxCnt_;
  ael::impl::dict::CumulativeUniqueCount zeroCtxUniqueCnt_;
  CtxCountMapping_ ctxInfo_;
};

}  // namespace ael::dict

#endif  // AEL_DICT_PPMA_DICTIONARY_HPP
