#include <ael/esc/dictionary/adaptive_d_dictionary.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <ranges>

namespace ael::esc::dict {

using boost::lambda::_1;
using boost::lambda::bind;
using std::ranges::upper_bound;

////////////////////////////////////////////////////////////////////////////////
AdaptiveDDictionary::AdaptiveDDictionary(const Ord maxOrd)
    : ael::impl::esc::dict::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordOrd(const Count cumulativeCnt) const -> Ord {
  if (escJustDecoded_) {
    return getWordOrdAfterEsc_(cumulativeCnt);
  }
  if (0 == getRealTotalWordsCnt_()) {
    return getMaxOrd_();
  }
  assert(cumulativeCnt < getRealTotalWordsCnt_() * 2 &&
         "Invalid cumulative count.");
  if (cumulativeCnt >=
      (getRealTotalWordsCnt_() * 2) - getTotalWordsUniqueCnt_()) {
    return getMaxOrd_();
  }
  return *upper_bound(getOrdRng_(), cumulativeCnt, {},
                      bind(&This_::getLowerCumulativeCnt_, this, _1));
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats(const Ord ord) -> StatsSeq {
  auto ret = getProbabilityStats_(ord);
  updateWordCnt_(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getDecodeProbabilityStats(const Ord ord)
    -> ProbabilityStats {
  auto ret = getDecodeProbabilityStats_(ord);
  if (!isEsc(ord)) {
    updateWordCnt_(ord, 1);
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getTotalWordsCnt() const -> Count {
  if (escJustDecoded_) {
    return getMaxOrd_() - getTotalWordsUniqueCnt_();
  }
  if (getRealTotalWordsCnt_() == 0) {
    return 1;
  }
  return getRealTotalWordsCnt_() * 2;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getLowerCumulativeCnt_(const Ord ord) const -> Count {
  return (getRealLowerCumulativeWordCnt_(ord + 1) * 2) -
         getLowerCumulativeUniqueNumFound_(ord + 1);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getLowerCumulativeCntAfterEsc_(const Ord ord) const
    -> Count {
  return ord + 1 - getLowerCumulativeUniqueNumFound_(ord + 1);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordOrdAfterEsc_(const Count cumulativeCnt) const
    -> Ord {
  return *upper_bound(getOrdRng_(), cumulativeCnt, {},
                      bind(&This_::getLowerCumulativeCntAfterEsc_, this, _1));
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats_(const Ord ord) const -> StatsSeq {
  if (getRealWordCnt_(ord) == 0) {
    return getProbabilityStatsForNewWord_(ord);
  }
  const auto symLow = (getRealLowerCumulativeWordCnt_(ord) * 2) -
                      getLowerCumulativeUniqueNumFound_(ord);
  const auto symHigh = symLow + (getRealWordCnt_(ord) * 2) - 1;
  const auto symTotal = getRealTotalWordsCnt_() * 2;

  return {{symLow, symHigh, symTotal}};
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getDecodeProbabilityStats_(const Ord ord)
    -> ProbabilityStats {
  if (0 == getRealTotalWordsCnt_() && !escJustDecoded_) {
    escJustDecoded_ = true;
    return {0, 1, 1};
  }
  if (isEsc(ord)) {
    escJustDecoded_ = true;
    const auto escLow =
        (2 * getRealTotalWordsCnt_()) - getTotalWordsUniqueCnt_();
    const auto escHigh = 2 * getRealTotalWordsCnt_();
    const auto escTotal = 2 * getRealTotalWordsCnt_();
    return {escLow, escHigh, escTotal};
  }
  if (escJustDecoded_) {
    escJustDecoded_ = false;
    const auto symLow = Count{ord} - getLowerCumulativeUniqueNumFound_(ord);
    const auto symHigh = symLow + 1;
    const auto symTotal = getMaxOrd_() - getTotalWordsUniqueCnt_();
    return {symLow, symHigh, symTotal};
  }
  const auto symLow = (2 * getRealLowerCumulativeWordCnt_(ord)) -
                      getLowerCumulativeUniqueNumFound_(ord);
  const auto symHigh = symLow + (2 * getRealWordCnt_(ord)) - 1;
  const auto symTotal = 2 * getRealTotalWordsCnt_();
  return {symLow, symHigh, symTotal};
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStatsForNewWord_(const Ord ord) const
    -> StatsSeq {
  auto ret = StatsSeq();
  if (getRealTotalWordsCnt_() == 0) [[unlikely]] {
    ret.emplace_back(Count{0}, Count{1}, Count{1});
  } else {
    const Count escLow =
        (getRealTotalWordsCnt_() * 2) - getTotalWordsUniqueCnt_();
    const Count escHigh = getRealTotalWordsCnt_() * 2;
    const Count escTotal = getRealTotalWordsCnt_() * 2;
    ret.emplace_back(escLow, escHigh, escTotal);
  }

  const Count symLow = Count{ord} - getLowerCumulativeUniqueNumFound_(ord);
  const Count symHigh = symLow + 1;
  const Count symTotal = getMaxOrd_() - getTotalWordsUniqueCnt_();
  ret.emplace_back(symLow, symHigh, symTotal);

  return ret;
}

}  // namespace ael::esc::dict
