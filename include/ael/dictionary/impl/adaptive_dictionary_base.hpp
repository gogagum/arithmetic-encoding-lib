#ifndef ADAPTIVE_DICTIONARY_BASE_HPP
#define ADAPTIVE_DICTIONARY_BASE_HPP

#include <cstdint>
#include <unordered_map>
#include <dst/dynamic_segment_tree.hpp>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionaryBase class
///
template <typename CountT>
class AdaptiveDictionaryBase {
protected:

    using Ord = std::uint64_t;
    using Count = CountT;
    constexpr const static std::uint16_t countNumBits = 62; 

protected:

    AdaptiveDictionaryBase(Ord maxOrd, Count initialTotalWordsCount)
        : _cumulativeWordCounts(Ord{0}, maxOrd, 0),
          _totalWordsCnt(initialTotalWordsCount) {}

protected:
    using DST_ =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;

protected:
    DST_ _cumulativeWordCounts;
    std::unordered_map<Ord, Count> _wordCnts;
    Count _totalWordsCnt;
};

}  // namespace ael::dict::impl

#endif // ADAPTIVE_DICTIONARY_BASE_HPP
