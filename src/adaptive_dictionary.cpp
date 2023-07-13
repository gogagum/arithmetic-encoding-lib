#include <ael/dictionary/adaptive_dictionary.hpp>
#include <ranges>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveDictionary::AdaptiveDictionary(ConstructInfo constructInfo)
    : impl::AdaptiveDictionaryBase<Count>(constructInfo.maxOrd,
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
  const auto high = low + this->getRealWordCnt_(ord) * ratio_ + 1;
  const auto total = getTotalWordsCnt();
  updateWordCnt_(ord);
  return {low, high, total};
}

////////////////////////////////////////////////////////////////////////////////
void AdaptiveDictionary::updateWordCnt_(Ord ord) {
  this->changeRealCumulativeWordCnt_(ord, 1);
  this->changeRealWordCnt_(ord, 1);
  this->changeRealTotalWordsCnt_(1);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  return (ord > Ord{0}) ? ord + this->getRealCumulativeCnt_(ord - 1) * ratio_
                        : Count{0};
}

}  // namespace ael::dict
