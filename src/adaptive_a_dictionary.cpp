#include <ael/dictionary/adaptive_a_dictionary.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <ranges>

namespace ael::dict {

using boost::lambda::_1;
using boost::lambda::bind;
using std::ranges::upper_bound;

////////////////////////////////////////////////////////////////////////////////
AdaptiveADictionary::AdaptiveADictionary(Ord maxOrd)
    : ael::impl::dict::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  return *upper_bound(getOrdRng_(), cumulativeCnt, {},
                      bind(&This_::getLowerCumulativeCnt_, this, _1 + 1));
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
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
  return {low, high, total};
}

}  // namespace ael::dict
