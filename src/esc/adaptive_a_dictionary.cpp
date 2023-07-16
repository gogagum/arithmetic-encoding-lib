#include <ael/dictionary/impl/a_d_dictionary_base.hpp>
#include <ael/esc/dictionary/adaptive_a_dictionary.hpp>
#include <ranges>

namespace ael::esc::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveADictionary::AdaptiveADictionary(Ord maxOrd)
    : ael::dict::impl::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  if (escJustDecoded_) {
    return getWordOrdAfterEsc_(cumulativeCnt);
  }
  assert(cumulativeCnt <= this->getRealTotalWordsCnt_());
  if (cumulativeCnt == this->getRealTotalWordsCnt_()) {
    escJustDecoded_ = true;
    return getMaxOrd_();
  }
  const auto idxs = std::ranges::iota_view(Ord{0}, this->getMaxOrd_());
  const auto getLowerCumulativeNumFound_ = [this](Ord ord) {
    return this->getRealLowerCumulativeWordCnt_(ord);
  };
  const auto iter = std::ranges::upper_bound(idxs, cumulativeCnt, {},
                                             getLowerCumulativeNumFound_);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats(Ord ord) -> StatsSeq {
  auto ret = getProbabilityStats_(ord);
  this->updateWordCnt_(ord, 1);
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
  const auto idxs = std::ranges::iota_view(Ord{0}, this->getMaxOrd_());
  const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
    return ord + 1 - this->getLowerCumulativeUniqueNumFound_(ord + 1);
  };
  const auto iter =
      std::ranges::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulNumFound_);
  escJustDecoded_ = false;
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats_(Ord ord) const -> StatsSeq {
  if (this->getRealWordCnt_(ord) == 0) {
    return getProbabilityStatsForNewWord_(ord);
  }

  const auto symLow = this->getRealLowerCumulativeWordCnt_(ord);
  const auto symHigh = symLow + this->getRealWordCnt_(ord);
  const auto symTotal = this->getRealTotalWordsCnt_() + 1;

  return {{symLow, symHigh, symTotal}};
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStatsForNewWord_(Ord ord) const
    -> StatsSeq {
  const auto escLow = this->getRealTotalWordsCnt_();
  const auto escHigh = escLow + 1;
  const auto escTotal = this->getRealTotalWordsCnt_() + 1;
  const auto escStats = ProbabilityStats{escLow, escHigh, escTotal};

  const auto symLow = Count{ord} - this->getLowerCumulativeUniqueNumFound_(ord);
  const auto symHigh = symLow + 1;
  const auto symTotal = getMaxOrd_() - this->getTotalWordsUniqueCnt_();
  const auto symStats = ProbabilityStats{symLow, symHigh, symTotal};

  return {escStats, symStats};
}

}  // namespace ael::esc::dict
