#include <ael/dictionary/adaptive_dictionary.hpp>
#include <ranges>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveDictionary::AdaptiveDictionary(ConstructInfo constructInfo)
    : ael::impl::dict::AdaptiveDictionaryBase<Count>(constructInfo.maxOrd,
                                                     constructInfo.maxOrd),
      ratio_(constructInfo.ratio),
      maxOrder_(constructInfo.maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
  const auto idxs = std::ranges::iota_view(Ord{0}, maxOrder_);
  const auto getLowerCumulNumFound_ = [this](Ord ord) {
    return getLowerCumulativeCnt_(ord + 1);
  };
  const auto iter = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  const auto low = getLowerCumulativeCnt_(ord);
  const auto high = low + getRealWordCnt_(ord) * ratio_ + 1;
  const auto total = getTotalWordsCnt();
  updateWordCnt_(ord);
  return {low, high, total};
}

////////////////////////////////////////////////////////////////////////////////
void AdaptiveDictionary::updateWordCnt_(Ord ord) {
  changeRealCumulativeWordCnt_(ord, 1);
  changeRealWordCnt_(ord, 1);
  changeRealTotalWordsCnt_(1);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  return (ord > Ord{0}) ? ord + getRealCumulativeCnt_(ord - 1) * ratio_
                        : Count{0};
}

}  // namespace ael::dict
