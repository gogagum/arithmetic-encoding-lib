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
    escJustDecoded_ = true;
    return getMaxOrd_();
  }
  const auto idxs = rng::iota_view(Ord{0}, getMaxOrd_());
  const auto getLowerCumulativeNumFound_ = [this](Ord ord) {
    return getRealLowerCumulativeWordCnt_(ord + 1);
  };
  const auto iter =
      rng::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulativeNumFound_);
  return iter - idxs.begin();
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
  const auto idxs = rng::iota_view(Ord{0}, getMaxOrd_());
  const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
    return ord + 1 - getLowerCumulativeUniqueNumFound_(ord + 1);
  };
  const auto iter =
      rng::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulNumFound_);
  return iter - idxs.begin();
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
auto AdaptiveADictionary::getDecodeProbabilityStats_(Ord ord) const
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
