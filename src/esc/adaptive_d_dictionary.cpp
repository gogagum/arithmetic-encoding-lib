#include <ael/esc/dictionary/adaptive_d_dictionary.hpp>
#include <ranges>

namespace ael::esc::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveDDictionary::AdaptiveDDictionary(Ord maxOrd)
    : ael::dict::impl::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  if (escJustDecoded_) {
    return getWordOrdAfterEsc_(cumulativeCnt);
  }
  if (0 == this->getRealTotalWordsCnt_()) {
    return getMaxOrd_();
  }
  assert(cumulativeCnt < this->getRealTotalWordsCnt_() * 2 &&
         "Invalid cumulative count.");
  if (cumulativeCnt >=
      this->getRealTotalWordsCnt_() * 2 - this->getTotalWordsUniqueCnt_()) {
    escJustDecoded_ = true;
    return getMaxOrd_();
  }
  const auto idxs = std::ranges::iota_view(Ord{0}, this->getMaxOrd_());
  const auto getLowerCumulativeNumFound_ = [this](Ord ord) {
    return this->getRealLowerCumulativeWordCnt_(ord) * 2 -
           this->getLowerCumulativeUniqueNumFound_(ord);
  };
  const auto iter = std::ranges::upper_bound(idxs, cumulativeCnt, {},
                                             getLowerCumulativeNumFound_);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats(Ord ord) -> StatsSeq {
  auto ret = getProbabilityStats_(ord);
  this->updateWordCnt_(ord, 1);
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
auto AdaptiveDDictionary::getWordOrdAfterEsc_(Count cumulativeCnt) const
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
auto AdaptiveDDictionary::getProbabilityStats_(Ord ord) const -> StatsSeq {
  if (this->getRealWordCnt_(ord) == 0) {
    return getProbabilityStatsForNewWord_(ord);
  }
  const auto symLow = this->getRealLowerCumulativeWordCnt_(ord) * 2 -
                      this->getLowerCumulativeUniqueNumFound_(ord);
  const auto symHigh = symLow + this->getRealWordCnt_(ord) * 2 - 1;
  const auto symTotal = this->getRealTotalWordsCnt_() * 2;

  return {{symLow, symHigh, symTotal}};
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStatsForNewWord_(Ord ord) const
    -> StatsSeq {
  auto ret = StatsSeq();
  if (getRealTotalWordsCnt_() == 0) [[unlikely]] {
    ret.push_back({Count{0}, Count{1}, Count{1}});
  } else {
    const auto escLow =
        this->getRealTotalWordsCnt_() * 2 - this->getTotalWordsUniqueCnt_();
    const auto escHigh = this->getRealTotalWordsCnt_() * 2;
    const auto escTotal = this->getRealTotalWordsCnt_() * 2;
    ret.push_back({escLow, escHigh, escTotal});
  }

  const auto symLow = Count{ord} - this->getLowerCumulativeUniqueNumFound_(ord);
  const auto symHigh = symLow + 1;
  const auto symTotal = getMaxOrd_() - this->getTotalWordsUniqueCnt_();
  ret.push_back({symLow, symHigh, symTotal});

  return ret;
}

}  // namespace ael::esc::dict