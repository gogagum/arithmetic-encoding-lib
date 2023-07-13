#include <ael/dictionary/decreasing_on_update_dictionary.hpp>
#include <algorithm>
#include <ranges>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
DecreasingOnUpdateDictionary::DecreasingOnUpdateDictionary(Ord maxOrd,
                                                           Count count)
    : impl::AdaptiveDictionaryBase<Count>(maxOrd, maxOrd * count),
      maxOrd_(maxOrd) {
  for (const auto ord : std::ranges::iota_view(Ord{0}, maxOrd_)) {
    this->changeRealWordCnt_(ord, static_cast<std::int64_t>(count));
    this->changeRealCumulativeWordCnt_(ord, static_cast<std::int64_t>(count));
  }
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::getWordOrd(Count cumulativeNumFound) const
    -> Ord {
  const auto idxs = std::ranges::iota_view(Ord{0}, maxOrd_);
  const auto getLowerCumulNumFound_ = [this](Ord ord) {
    return this->getLowerCumulativeCnt_(ord + 1);
  };
  const auto iter = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::getProbabilityStats(Ord ord)
    -> ProbabilityStats {
  auto ret = getProbabilityStats_(ord);
  updateWordCnt_(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::getLowerCumulativeCnt_(Ord ord) const
    -> Count {
  return getRealCumulativeCnt_(ord - 1);
}

////////////////////////////////////////////////////////////////////////////////
void DecreasingOnUpdateDictionary::updateWordCnt_(Ord ord, Count cnt) {
  this->changeRealTotalWordsCnt_(-1);
  this->changeRealCumulativeWordCnt_(ord, -static_cast<std::int64_t>(cnt));
  this->changeRealWordCnt_(ord, -1);
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::getProbabilityStats_(Ord ord) const
    -> ProbabilityStats {
  assert(this->getRealWordCnt_(ord) != Count{0} &&
         "Get probability stats for a word woth zero real count.");
  const auto low = getLowerCumulativeCnt_(ord);
  const auto high = low + getRealWordCnt_(ord);
  const auto total = getTotalWordsCnt();
  return {low, high, total};
}

}  // namespace ael::dict
