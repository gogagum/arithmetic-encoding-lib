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
        : cumulativeWordCounts_(Ord{0}, maxOrd, 0),
          totalWordsCnt_(initialTotalWordsCount),
          maxOrd_{maxOrd} {}

    [[nodiscard]] Count getRealWordCnt_(Ord ord) const {
      return wordCnts_.contains(ord) ? wordCnts_.at(ord) : 0;
    }

    void changeRealCumulativeWordCnt_(Ord ord, std::int64_t cntChange) {
      cumulativeWordCounts_.update(ord, maxOrd_, cntChange);
    }

    void changeRealWordCnt_(Ord ord, std::int64_t cntChange) {
      wordCnts_[ord] += cntChange;
    }

    void changeRealTotalWordsCnt_(std::int64_t cntChange) {
      totalWordsCnt_ += cntChange;
    }

    [[nodiscard]] Count getRealTotalWordsCnt_() const {
      return totalWordsCnt_;
    }

    [[nodiscard]] Count getRealCumulativeCnt_(Ord ord) const {
      return cumulativeWordCounts_.get(ord);
    }

protected:
    using DST_ =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;

private:
    DST_ cumulativeWordCounts_;
    std::unordered_map<Ord, Count> wordCnts_{};
    Count totalWordsCnt_;
    Ord maxOrd_;
};

}  // namespace ael::dict::impl

#endif // ADAPTIVE_DICTIONARY_BASE_HPP
