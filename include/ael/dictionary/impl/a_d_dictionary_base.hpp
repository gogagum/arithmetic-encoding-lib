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

    ADDictionaryBase(Ord maxOrd);

    Count _getRealTotalWordsCnt() const;

    Count _getRealLowerCumulativeWordCnt(Ord ord) const;

    Count _getRealWordCnt(Ord ord) const;

    Count _getTotalWordsUniqueCnt() const;

    Count _getLowerCumulativeUniqueNumFound(Ord ord) const;

    Count _getWordUniqueCnt(Ord ord) const;

    void _updateWordCnt(Ord ord, Count cnt);

protected:
    impl::CumulativeCount _cumulativeCnt;
    impl::CumulativeUniqueCount _cumulativeUniqueCnt;
    const std::uint64_t _maxOrd;
};

}  // namespace ael::dict::impl

#endif // A_D_DICTIONARY_BASE_HPP
