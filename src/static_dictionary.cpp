#include <ael/dictionary/static_dictionary.hpp>
#include <algorithm>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
auto StaticDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
  const auto iter =
      std::ranges::upper_bound(cumulativeNumFound_, cumulativeNumFound);
  return iter - cumulativeNumFound_.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto StaticDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  const auto low = getLowerCumulativeCnt_(ord);
  const auto high = getHigherCumulativeCnt_(ord);
  return {low, high, *cumulativeNumFound_.rbegin()};
}

////////////////////////////////////////////////////////////////////////////////
auto StaticDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  return ord != 0 ? cumulativeNumFound_[ord - 1] : 0;
}

}  // namespace ael::dict
