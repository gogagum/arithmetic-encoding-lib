#ifndef AEL_DICT_PPMD_DICTIONARY_HPP
#define AEL_DICT_PPMD_DICTIONARY_HPP

#include <ael/impl/dictionary/cumulative_count.hpp>
#include <ael/impl/dictionary/cumulative_unique_count.hpp>
#include <ael/impl/dictionary/ppm_a_d_dictionary_base.hpp>
#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstddef>
#include <cstdint>
#include <unordered_map>

namespace ael::dict {

constexpr auto ppmdMaxCtxLength = std::uint16_t{16};

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMDDictionary - ppmd probability model.
///
class PPMDDictionary
    : ael::impl::dict::PPMADDictionaryBase<PPMDDictionary, ppmdMaxCtxLength> {
 private:
  using This_ = PPMDDictionary;

 protected:
  using Base_ =
      ael::impl::dict::PPMADDictionaryBase<PPMDDictionary, ppmdMaxCtxLength>;
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
  constexpr static std::uint16_t maxSeqLenLog2_ = 40;

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
  using SearchCtx_ = Base_::SearchCtx_;
  using SearchCtxHash_ = boost::hash<SearchCtx_>;
  struct CtxCell_ {
    explicit CtxCell_(Ord maxOrd) : cnt(maxOrd), uniqueCnt(maxOrd) {
    }
    CumulativeCount_ cnt;
    CumulativeUniqueCount_ uniqueCnt;
  };
  using CtxCountMapping_ =
      std::unordered_map<SearchCtx_, CtxCell_, SearchCtxHash_>;

 private:
  [[nodiscard]] Count getLowerCumulativeCnt_(Ord ord) const;

  [[nodiscard]] ProbabilityStats getProbabilityStats_(Ord ord) const;

  void updateWordCnt_(Ord ord, std::int64_t cnt);

 private:
  CtxCell_ zeroCtxCell_;
  CtxCountMapping_ ctxInfo_;

 private:
  template <class DictT, typename CountT, std::uint16_t maxCtxLength>
  friend class ael::impl::dict::CtxBase;
};

}  // namespace ael::dict

#endif  // AEL_DICT_PPMD_DICTIONARY_HPP
