#include <ael/dictionary/adaptive_dictionary.hpp>
#include <ranges>

namespace ael::dict {

namespace rng = std::ranges;

////////////////////////////////////////////////////////////////////////////////
AdaptiveDictionary::AdaptiveDictionary(ConstructInfo constructInfo)
    : ael::impl::dict::AdaptiveDictionaryBase<Count>(constructInfo.maxOrd,
                                                     constructInfo.maxOrd),
      ratio_(constructInfo.ratio),
      maxOrder_(constructInfo.maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  const auto getLowerCumulCnt_ = [this](Ord ord) {
    return getLowerCumulativeCnt_(ord + 1);
  };
  return *rng::upper_bound(this->getOrdRng_(), cumulativeCnt, {}, getLowerCumulCnt_);
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
