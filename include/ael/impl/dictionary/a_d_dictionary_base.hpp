#ifndef A_D_DICTIONARY_BASE_HPP
#define A_D_DICTIONARY_BASE_HPP

#include <ael/impl/dictionary/cumulative_count.hpp>
#include <ael/impl/dictionary/cumulative_unique_count.hpp>
#include <cstdint>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BaseADDictionary class
///
class ADDictionaryBase {
 protected:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;

 protected:
  explicit ADDictionaryBase(Ord maxOrd);

  [[nodiscard]] Count getRealTotalWordsCnt_() const;

  [[nodiscard]] Count getRealLowerCumulativeWordCnt_(Ord ord) const;

  [[nodiscard]] Count getRealWordCnt_(Ord ord) const;

  [[nodiscard]] Count getTotalWordsUniqueCnt_() const;

  [[nodiscard]] Count getLowerCumulativeUniqueNumFound_(Ord ord) const;

  [[nodiscard]] Count getWordUniqueCnt_(Ord ord) const;

  void updateWordCnt_(Ord ord, Count cnt);

  [[nodiscard]] Ord getMaxOrd_() const {
    return maxOrd_;
  }

 private:
  CumulativeCount cumulativeCnt_;
  CumulativeUniqueCount cumulativeUniqueCnt_;
  const std::uint64_t maxOrd_;
};

}  // namespace ael::impl::dict

#endif  // A_D_DICTIONARY_BASE_HPP
