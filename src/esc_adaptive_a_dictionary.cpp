#include <ael/esc/dictionary/adaptive_a_dictionary.hpp>
#include <ranges>

namespace ael::esc::dict {

namespace rng = std::ranges;

////////////////////////////////////////////////////////////////////////////////
AdaptiveADictionary::AdaptiveADictionary(Ord maxOrd)
    : ael::impl::esc::dict::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  if (escJustDecoded_) {
    return getWordOrdAfterEsc_(cumulativeCnt);
  }
  assert(cumulativeCnt < getRealTotalWordsCnt_() + 1);
  if (cumulativeCnt == getRealTotalWordsCnt_()) {
    return getMaxOrd_();
  }
  const auto getLowerCumulCnt_ = [this](Ord ord) {
    return getRealLowerCumulativeWordCnt_(ord + 1);
  };
  return *rng::upper_bound(getOrdRng_(), cumulativeCnt, {}, getLowerCumulCnt_);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats(Ord ord) -> StatsSeq {
  auto ret = getProbabilityStats_(ord);
  updateWordCnt_(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getDecodeProbabilityStats(Ord ord)
    -> ProbabilityStats {
  auto ret = getDecodeProbabilityStats_(ord);
  if (!isEsc(ord)) {
    updateWordCnt_(ord, 1);
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getTotalWordsCnt() const -> Count {
  if (escJustDecoded_) {
    return getMaxOrd_() - getTotalWordsUniqueCnt_();
  }
  return getRealTotalWordsCnt_() + 1;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordOrdAfterEsc_(Count cumulativeCnt) const
    -> Ord {
  const auto getLowerCumulCnt_ = [this](Ord ord) {
    return ord + 1 - getLowerCumulativeUniqueNumFound_(ord + 1);
  };
  return *rng::upper_bound(getOrdRng_(), cumulativeCnt, {}, getLowerCumulCnt_);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats_(Ord ord) const -> StatsSeq {
  assert(!isEsc(ord) && "Ord can not be esc here.");
  if (getRealWordCnt_(ord) == 0) {
    return getProbabilityStatsForNewWord_(ord);
  }

  const auto symLow = getRealLowerCumulativeWordCnt_(ord);
  const auto symHigh = symLow + getRealWordCnt_(ord);
  const auto symTotal = getRealTotalWordsCnt_() + 1;

  return {{symLow, symHigh, symTotal}};
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStatsForNewWord_(Ord ord) const
    -> StatsSeq {
  const auto escLow = getRealTotalWordsCnt_();
  const auto escHigh = escLow + 1;
  const auto escTotal = getRealTotalWordsCnt_() + 1;
  const auto escStats = ProbabilityStats{escLow, escHigh, escTotal};

  const auto symLow = Count{ord} - getLowerCumulativeUniqueNumFound_(ord);
  const auto symHigh = symLow + 1;
  const auto symTotal = getMaxOrd_() - getTotalWordsUniqueCnt_();
  const auto symStats = ProbabilityStats{symLow, symHigh, symTotal};

  return {escStats, symStats};
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getDecodeProbabilityStats_(Ord ord)
    -> ProbabilityStats {
  if (isEsc(ord)) {
    escJustDecoded_ = true;
    const auto escLow = getRealTotalWordsCnt_();
    const auto escHigh = escLow + 1;
    const auto escTotal = getRealTotalWordsCnt_() + 1;
    return {escLow, escHigh, escTotal};
  }
  if (escJustDecoded_) {
    escJustDecoded_ = false;
    const auto symLow = Count{ord} - getLowerCumulativeUniqueNumFound_(ord);
    const auto symHigh = symLow + 1;
    const auto symTotal = getMaxOrd_() - getTotalWordsUniqueCnt_();
    return {symLow, symHigh, symTotal};
  }
  const auto symLow = getRealLowerCumulativeWordCnt_(ord);
  const auto symHigh = symLow + getRealWordCnt_(ord);
  const auto symTotal = getRealTotalWordsCnt_() + 1;
  return {symLow, symHigh, symTotal};
}

}  // namespace ael::esc::dict
