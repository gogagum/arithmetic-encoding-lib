#include <ael/dictionary/decreasing_on_update_dictionary.hpp>
#include <algorithm>
#include <ranges>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
DecreasingOnUpdateDictionary::DecreasingOnUpdateDictionary(Ord maxOrd,
                                                           Count count)
    : ael::impl::dict::AdaptiveDictionaryBase<Count>(maxOrd, maxOrd * count),
      maxOrd_(maxOrd) {
  for (const auto ord : std::ranges::iota_view(Ord{0}, maxOrd_)) {
    changeRealWordCnt_(ord, static_cast<std::int64_t>(count));
    changeRealCumulativeWordCnt_(ord, static_cast<std::int64_t>(count));
  }
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::getWordOrd(Count cumulativeNumFound) const
    -> Ord {
  const auto idxs = std::ranges::iota_view(Ord{0}, maxOrd_);
  const auto getLowerCumulNumFound_ = [this](Ord ord) {
    return getLowerCumulativeCnt_(ord + 1);
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
void DecreasingOnUpdateDictionary::updateWordCnt_(Ord ord, Count cnt) {
  changeRealTotalWordsCnt_(-1);
  changeRealCumulativeWordCnt_(ord, -static_cast<std::int64_t>(cnt));
  changeRealWordCnt_(ord, -1);
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::getProbabilityStats_(Ord ord) const
    -> ProbabilityStats {
  assert(getRealWordCnt_(ord) != Count{0} &&
         "Get probability stats for a word woth zero real count.");
  const auto low = getLowerCumulativeCnt_(ord);
  const auto high = low + getRealWordCnt_(ord);
  const auto total = getTotalWordsCnt();
  return {low, high, total};
}

}  // namespace ael::dict
