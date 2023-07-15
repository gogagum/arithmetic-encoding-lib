#ifndef ADAPTIVE_DICTIONARY_BASE_HPP
#define ADAPTIVE_DICTIONARY_BASE_HPP

#include <cstdint>
#include <dst/dynamic_segment_tree.hpp>
#include <unordered_map>

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
  AdaptiveDictionaryBase(Ord maxOrd, Count initialTotalWordsCount);

  [[nodiscard]] Count getRealWordCnt_(Ord ord) const;

  void changeRealCumulativeWordCnt_(Ord ord, std::int64_t cntChange);

  void changeRealWordCnt_(Ord ord, std::int64_t cntChange);

  void changeRealTotalWordsCnt_(std::int64_t cntChange);

  [[nodiscard]] Count getRealTotalWordsCnt_() const;

  [[nodiscard]] Count getRealCumulativeCnt_(Ord ord) const;

 protected:
  using DST_ =
      dst::DynamicSegmentTree<Ord, Count, void, dst::NoRangeGetOp,
                              dst::NoRangeGetOp, std::plus<void>, std::int64_t>;

 private:
  DST_ cumulativeWordCounts_;
  std::unordered_map<Ord, Count> wordCnts_{};
  Count totalWordsCnt_;
  Ord maxOrd_;
};

////////////////////////////////////////////////////////////////////////////////
template <typename CountT>
AdaptiveDictionaryBase<CountT>::AdaptiveDictionaryBase(
    Ord maxOrd, Count initialTotalWordsCount)
    : cumulativeWordCounts_{Ord{0}, maxOrd, 0},
      totalWordsCnt_{initialTotalWordsCount},
      maxOrd_{maxOrd} {
}

////////////////////////////////////////////////////////////////////////////////
template <typename CountT>
auto AdaptiveDictionaryBase<CountT>::getRealWordCnt_(Ord ord) const -> Count {
  return wordCnts_.contains(ord) ? wordCnts_.at(ord) : 0;
}

////////////////////////////////////////////////////////////////////////////////
template <typename CountT>
void AdaptiveDictionaryBase<CountT>::changeRealCumulativeWordCnt_(
    Ord ord, std::int64_t cntChange) {
  cumulativeWordCounts_.update(ord, maxOrd_, cntChange);
}

////////////////////////////////////////////////////////////////////////////////
template <typename CountT>
void AdaptiveDictionaryBase<CountT>::changeRealWordCnt_(
    Ord ord, std::int64_t cntChange) {
  wordCnts_[ord] += cntChange;
}

////////////////////////////////////////////////////////////////////////////////
template <typename CountT>
void AdaptiveDictionaryBase<CountT>::changeRealTotalWordsCnt_(
    std::int64_t cntChange) {
  totalWordsCnt_ += cntChange;
}

////////////////////////////////////////////////////////////////////////////////
template <typename CountT>
auto AdaptiveDictionaryBase<CountT>::getRealTotalWordsCnt_() const -> Count {
  return totalWordsCnt_;
}

////////////////////////////////////////////////////////////////////////////////
template <typename CountT>
auto AdaptiveDictionaryBase<CountT>::getRealCumulativeCnt_(Ord ord) const
    -> Count {
  return cumulativeWordCounts_.get(ord);
}

}  // namespace ael::dict::impl

#endif  // ADAPTIVE_DICTIONARY_BASE_HPP
