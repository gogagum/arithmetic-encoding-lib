#ifndef A_D_DICTIONARY_BASE_HPP
#define A_D_DICTIONARY_BASE_HPP

#include <ael/dictionary/impl/cumulative_count.hpp>
#include <ael/dictionary/impl/cumulative_unique_count.hpp>
#include <cstdint>

namespace ael::dict::impl {

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
  impl::CumulativeCount cumulativeCnt_;
  impl::CumulativeUniqueCount cumulativeUniqueCnt_;
  const std::uint64_t maxOrd_;
};

}  // namespace ael::dict::impl

#endif  // A_D_DICTIONARY_BASE_HPP
