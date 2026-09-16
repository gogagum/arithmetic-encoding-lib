#include <ael/dictionary/adaptive_a_dictionary.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <ranges>

namespace ael::dict {

using boost::lambda::_1;
using boost::lambda::bind;
using std::ranges::upper_bound;

////////////////////////////////////////////////////////////////////////////////
AdaptiveADictionary::AdaptiveADictionary(const Ord maxOrd)
    : ael::impl::dict::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordOrd(const Count cumulativeCnt) const -> Ord {
  return *upper_bound(getOrdRng_(), cumulativeCnt, {},
                      bind(&This_::getLowerCumulativeCnt_, this, _1 + 1));
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats(const Ord ord) -> ProbabilityStats {
  const ProbabilityStats ret = getProbabilityStats_(ord);
  updateWordCnt_(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getTotalWordsCnt() const -> Count {
  const Count uniqueWordsCnt = getTotalWordsUniqueCnt_();
  const Count wordsCnt = getRealTotalWordsCnt_();
  if (getMaxOrd_() == uniqueWordsCnt) {
    return wordsCnt;
  }
  return (getMaxOrd_() - uniqueWordsCnt) * (wordsCnt + 1);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getLowerCumulativeCnt_(const Ord ord) const -> Count {
  const Count cumulativeNumFound = getRealLowerCumulativeWordCnt_(ord);
  if (getMaxOrd_() == getTotalWordsUniqueCnt_()) {
    return cumulativeNumFound;
  }
  const Count numUniqueWordsTotal = getTotalWordsUniqueCnt_();
  const Count cumulativeUniqueWordsNumFound =
      getLowerCumulativeUniqueNumFound_(ord);
  return ((getMaxOrd_() - numUniqueWordsTotal) * cumulativeNumFound) +
         (ord - cumulativeUniqueWordsNumFound);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordCnt_(const Ord ord) const -> Count {
  const Count totalUniqueWordsCnt = getTotalWordsUniqueCnt_();
  if (getMaxOrd_() == totalUniqueWordsCnt) {
    return getRealWordCnt_(ord);
  }
  return getWordUniqueCnt_(ord) == 1
             ? getRealWordCnt_(ord) * (getMaxOrd_() - totalUniqueWordsCnt)
             : 1;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats_(const Ord ord) const
    -> ProbabilityStats {
  const Count low = getLowerCumulativeCnt_(ord);
  return {
      .low = low,
      .high = low + getTotalWordsCnt(),
      .total = getTotalWordsCnt(),
  };
}

}  // namespace ael::dict
