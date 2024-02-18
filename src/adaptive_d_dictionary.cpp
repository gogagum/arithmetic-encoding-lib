#include <ael/dictionary/adaptive_d_dictionary.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <ranges>

namespace ael::dict {

using boost::lambda::_1;
using boost::lambda::bind;
using std::ranges::upper_bound;

////////////////////////////////////////////////////////////////////////////////
AdaptiveDDictionary::AdaptiveDDictionary(Ord maxOrd)
    : ael::impl::dict::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  return *upper_bound(getOrdRng_(), cumulativeCnt, {},
                      bind(&This_::getLowerCumulativeCnt_, this, _1 + 1));
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  const auto ret = getProbabilityStats_(ord);
  updateWordCnt_(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getTotalWordsCnt() const -> Count {
  const auto totalWordsCnt = getRealTotalWordsCnt_();
  if (totalWordsCnt == 0) {
    return getMaxOrd_();
  }
  const auto totalWordsUniqueCnt = getTotalWordsUniqueCnt_();
  if (totalWordsUniqueCnt == getMaxOrd_()) {
    return totalWordsCnt;
  }
  return 2 * (getMaxOrd_() - totalWordsUniqueCnt) * totalWordsCnt;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  if (getRealTotalWordsCnt_() == 0) {
    return ord;
  }
  const auto totalUniqueWordsCnt = getTotalWordsUniqueCnt_();
  const auto cumulativeWordsCnt = getRealLowerCumulativeWordCnt_(ord);
  if (totalUniqueWordsCnt == getMaxOrd_()) {
    return cumulativeWordsCnt;
  }
  return (getMaxOrd_() - totalUniqueWordsCnt) * 2 * cumulativeWordsCnt +
         ord * totalUniqueWordsCnt -
         getMaxOrd_() * getLowerCumulativeUniqueNumFound_(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordCnt_(Ord ord) const -> Count {
  if (getRealTotalWordsCnt_() == 0) {
    return 1;
  }
  const auto totalUniqueWordsCount = getTotalWordsUniqueCnt_();
  const auto realWordCnt = getRealWordCnt_(ord);
  if (totalUniqueWordsCount == getMaxOrd_()) {
    return realWordCnt;
  }
  return (getMaxOrd_() - totalUniqueWordsCount) * 2 * realWordCnt +
         totalUniqueWordsCount - getMaxOrd_() * getWordUniqueCnt_(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats_(Ord ord) const
    -> ProbabilityStats {
  const auto low = getLowerCumulativeCnt_(ord);
  const auto high = low + getWordCnt_(ord);
  const auto total = getTotalWordsCnt();
  return {low, high, total};
}

}  // namespace ael::dict
