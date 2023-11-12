#include <ael/dictionary/static_dictionary.hpp>
#include <algorithm>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
StaticDictionary::StaticDictionary(Ord maxOrd,
                                   const std::map<Ord, Count>& countsMapping) {
  cumulativeNumFound_.resize(maxOrd);
  auto currOrd = Ord{0};
  auto currCumulativeNumFound = Count{0};
  for (const auto& [ord, count] : countsMapping) {
    for (; currOrd < ord; ++currOrd) {
      cumulativeNumFound_[currOrd] = currCumulativeNumFound;
    }
    currCumulativeNumFound += count;
  }
  for (; currOrd < maxOrd; ++currOrd) {
    cumulativeNumFound_[currOrd] = currCumulativeNumFound;
  }
}

////////////////////////////////////////////////////////////////////////////////
auto StaticDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
  return std::ranges::upper_bound(cumulativeNumFound_, cumulativeNumFound) -
         cumulativeNumFound_.begin();
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
