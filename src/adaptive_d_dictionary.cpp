#include <ael/dictionary/adaptive_d_dictionary.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <ranges>

namespace ael::dict {

using boost::lambda::_1;
using boost::lambda::bind;
using std::ranges::upper_bound;

////////////////////////////////////////////////////////////////////////////////
AdaptiveDDictionary::AdaptiveDDictionary(const Ord maxOrd)
    : ael::impl::dict::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordOrd(const Count cumulativeCnt) const -> Ord {
  return *upper_bound(getOrdRng_(), cumulativeCnt, {},
                      bind(&This_::getLowerCumulativeCnt_, this, _1 + 1));
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats(const Ord ord) -> ProbabilityStats {
  const ProbabilityStats ret = getProbabilityStats_(ord);
  updateWordCnt_(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getTotalWordsCnt() const -> Count {
  const Count totalWordsCnt = getRealTotalWordsCnt_();
  if (totalWordsCnt == 0) {
    return getMaxOrd_();
  }
  const Count totalWordsUniqueCnt = getTotalWordsUniqueCnt_();
  if (totalWordsUniqueCnt == getMaxOrd_()) {
    return totalWordsCnt;
  }
  return 2 * (getMaxOrd_() - totalWordsUniqueCnt) * totalWordsCnt;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getLowerCumulativeCnt_(const Ord ord) const -> Count {
  if (getRealTotalWordsCnt_() == 0) {
    return ord;
  }
  const Count totalUniqueWordsCnt = getTotalWordsUniqueCnt_();
  const Count cumulativeWordsCnt = getRealLowerCumulativeWordCnt_(ord);
  if (totalUniqueWordsCnt == getMaxOrd_()) {
    return cumulativeWordsCnt;
  }
  return ((getMaxOrd_() - totalUniqueWordsCnt) * 2 * cumulativeWordsCnt) +
         (ord * totalUniqueWordsCnt) -
         (getMaxOrd_() * getLowerCumulativeUniqueNumFound_(ord));
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordCnt_(const Ord ord) const -> Count {
  if (getRealTotalWordsCnt_() == 0) {
    return 1;
  }
  const Count totalUniqueWordsCount = getTotalWordsUniqueCnt_();
  const Count realWordCnt = getRealWordCnt_(ord);
  if (totalUniqueWordsCount == getMaxOrd_()) {
    return realWordCnt;
  }
  return ((getMaxOrd_() - totalUniqueWordsCount) * 2 * realWordCnt) +
         totalUniqueWordsCount - (getMaxOrd_() * getWordUniqueCnt_(ord));
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats_(const Ord ord) const
    -> ProbabilityStats {
  const Count low = getLowerCumulativeCnt_(ord);
  return {
      .low = low,
      .high = low + getWordCnt_(ord),
      .total = getTotalWordsCnt(),
  };
}

}  // namespace ael::dict
