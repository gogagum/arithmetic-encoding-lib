#include <ael/dictionary/static_dictionary.hpp>
#include <algorithm>
#include <ranges>

namespace ael::dict {

using std::ranges::fill;
using std::ranges::upper_bound;
using std::views::drop;
using std::views::take;

////////////////////////////////////////////////////////////////////////////////
StaticDictionary::StaticDictionary(const Ord maxOrd,
                                   const std::map<Ord, Count>& countsMapping) {
  cumulativeNumFound_.resize(maxOrd);
  auto currOrd = Ord{0};
  auto currCumulCnt = Count{0};
  for (const auto& [ord, count] : countsMapping) {
    fill(cumulativeNumFound_ | drop(currOrd) | take(ord - currOrd),
         currCumulCnt);
    currOrd = ord;
    currCumulCnt += count;
  }
  fill(cumulativeNumFound_ | drop(currOrd), currCumulCnt);
}

////////////////////////////////////////////////////////////////////////////////
auto StaticDictionary::getWordOrd(const Count cumulativeNumFound) const -> Ord {
  return upper_bound(cumulativeNumFound_, cumulativeNumFound) -
         cumulativeNumFound_.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto StaticDictionary::getProbabilityStats(const Ord ord) -> ProbabilityStats {
  return {
      .low = getLowerCumulativeCnt_(ord),
      .high = getHigherCumulativeCnt_(ord),
      .total = cumulativeNumFound_.back(),
  };
}

////////////////////////////////////////////////////////////////////////////////
auto StaticDictionary::getLowerCumulativeCnt_(const Ord ord) const -> Count {
  return ord != 0 ? cumulativeNumFound_[ord - 1] : 0;
}

}  // namespace ael::dict
