#ifndef A_D_DICTIONARY_BASE_HPP
#define A_D_DICTIONARY_BASE_HPP

#include <cstdint>

#include "cumulative_count.hpp"
#include "cumulative_unique_count.hpp"

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

  [[nodiscard]] Count _getRealTotalWordsCnt() const;

  [[nodiscard]] Count _getRealLowerCumulativeWordCnt(Ord ord) const;

  [[nodiscard]] Count _getRealWordCnt(Ord ord) const;

  [[nodiscard]] Count _getTotalWordsUniqueCnt() const;

  [[nodiscard]] Count _getLowerCumulativeUniqueNumFound(Ord ord) const;

  [[nodiscard]] Count _getWordUniqueCnt(Ord ord) const;

  void _updateWordCnt(Ord ord, Count cnt);

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
