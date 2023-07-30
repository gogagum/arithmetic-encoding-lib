#include <ael/dictionary/adaptive_a_dictionary.hpp>
#include <ranges>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveADictionary::AdaptiveADictionary(Ord maxOrd)
    : ael::impl::dict::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  const auto idxs = std::ranges::iota_view(Ord{0}, getMaxOrd_());
  const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
    return getLowerCumulativeCnt_(ord + 1);
  };
  const auto iter =
      std::ranges::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulNumFound_);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  assert(ord < getMaxOrd_() && "ord is out of range");
  const auto ret = getProbabilityStats_(ord);
  updateWordCnt_(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getTotalWordsCnt() const -> Count {
  const auto uniqueWordsCnt = getTotalWordsUniqueCnt_();
  const auto wordsCnt = getRealTotalWordsCnt_();
  if (getMaxOrd_() == uniqueWordsCnt) {
    return wordsCnt;
  }
  return (getMaxOrd_() - uniqueWordsCnt) * (wordsCnt + 1);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  const auto cumulativeNumFound = getRealLowerCumulativeWordCnt_(ord);
  if (getMaxOrd_() == getTotalWordsUniqueCnt_()) {
    return cumulativeNumFound;
  }
  const auto numUniqueWordsTotal = getTotalWordsUniqueCnt_();
  const auto cumulativeUniqueWordsNumFound =
      getLowerCumulativeUniqueNumFound_(ord);
  return (getMaxOrd_() - numUniqueWordsTotal) * cumulativeNumFound +
         (ord - cumulativeUniqueWordsNumFound);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordCnt_(Ord ord) const -> Count {
  const auto totalUniqueWordsCnt = getTotalWordsUniqueCnt_();
  if (getMaxOrd_() == totalUniqueWordsCnt) {
    return getRealWordCnt_(ord);
  }
  return getWordUniqueCnt_(ord) == 1
             ? getRealWordCnt_(ord) * (getMaxOrd_() - totalUniqueWordsCnt)
             : 1;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats_(Ord ord) const
    -> ProbabilityStats {
  const auto low = getLowerCumulativeCnt_(ord);
  const auto high = low + getWordCnt_(ord);
  const auto total = getTotalWordsCnt();
  assert(high > low);
  assert(total >= high);
  return {low, high, total};
}

}  // namespace ael::dict
