#include <ael/esc/dictionary/ppmd_dictionary.hpp>
#include <algorithm>
#include <ranges>

namespace ael::esc::dict {

namespace rng = std::ranges;

////////////////////////////////////////////////////////////////////////////////
PPMDDictionary::PPMDDictionary(ConstructInfo constructInfo)
    : ael::impl::esc::dict::PPMADDictionaryBase(constructInfo.maxOrd),
      zeroCtxCnt_(constructInfo.maxOrd),
      zeroCtxUniqueCnt_(constructInfo.maxOrd),
      ctxLength_(constructInfo.ctxLength) {
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  const auto idxs = rng::iota_view{Ord{0}, getMaxOrd_()};
  auto currCtx = SearchCtx_(ctx_.rbegin(), ctx_.rend());
  skipNewCtxs_(currCtx);
  if (getEscDecoded_() < currCtx.size()) {
    skipCtxsByEsc_(currCtx);
    const auto& currCtxInfo = ctxInfo_.at(currCtx);
    const auto getLowerCumulCnt = [&currCtxInfo](Ord ord) {
      return 2 * currCtxInfo.cnt.getLowerCumulativeCount(ord + 1) -
             currCtxInfo.uniqueCnt.getLowerCumulativeCount(ord + 1);
    };
    return rng::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulCnt) -
           idxs.begin();
  }
  if (getEscDecoded_() == currCtx.size()) {
    if (0 == zeroCtxCnt_.getTotalWordsCnt()) {
      return getMaxOrd_();
    }
    const auto getLowerCumulCnt = [this](Ord ord) {
      return 2 * zeroCtxCnt_.getLowerCumulativeCount(ord + 1) -
             zeroCtxUniqueCnt_.getLowerCumulativeCount(ord + 1);
    };
    return rng::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulCnt) -
           idxs.begin();
  }
  assert(getEscDecoded_() == currCtx.size() + 1 &&
         "Esc decoded  count can not be that big.");
  return getWordOrdForNewWord_(cumulativeCnt);
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getProbabilityStats(Ord ord) -> StatsSeq {
  StatsSeq ret;
  auto currCtx = SearchCtx_(ctx_.rbegin(), ctx_.rend());
  updateCtx_(ord);
  for (; !currCtx.empty() && !ctxInfo_.contains(currCtx); currCtx.pop_back()) {
    ctxInfo_.emplace(currCtx,
                     CtxCell_{impl::dict::CumulativeCount(getMaxOrd_()),
                              impl::dict::CumulativeUniqueCount(getMaxOrd_())});
    ctxInfo_.at(currCtx).cnt.increaseOrdCount(ord, 1);
    ctxInfo_.at(currCtx).uniqueCnt.update(ord);
  }
  for (; !currCtx.empty() && ctxInfo_.at(currCtx).cnt.getCount(ord) == 0;
       currCtx.pop_back()) {
    const auto escLow = 2 * ctxInfo_.at(currCtx).cnt.getTotalWordsCnt() -
                        ctxInfo_.at(currCtx).uniqueCnt.getTotalWordsCnt();
    const auto escHigh =
        escLow + ctxInfo_.at(currCtx).uniqueCnt.getTotalWordsCnt();
    const auto escTotal = 2 * ctxInfo_.at(currCtx).cnt.getTotalWordsCnt();
    ret.emplace_back(escLow, escHigh, escTotal);
    ctxInfo_.at(currCtx).cnt.increaseOrdCount(ord, 1);
    ctxInfo_.at(currCtx).uniqueCnt.update(ord);
  }
  if (currCtx.empty()) {
    if (0 == zeroCtxCnt_.getCount(ord)) {
      const auto escLow = 2 * zeroCtxCnt_.getTotalWordsCnt() -
                          zeroCtxUniqueCnt_.getTotalWordsCnt();
      const auto escHigh =
          escLow + ((0 == zeroCtxCnt_.getTotalWordsCnt())
                        ? Count{1}
                        : zeroCtxUniqueCnt_.getTotalWordsCnt());
      const auto escTotal = (0 == zeroCtxCnt_.getTotalWordsCnt())
                                ? 1
                                : (2 * zeroCtxCnt_.getTotalWordsCnt());
      ret.emplace_back(escLow, escHigh, escTotal);
      const auto symLow =
          Count{ord} - zeroCtxUniqueCnt_.getLowerCumulativeCount(ord);
      const auto symHigh = symLow + 1;
      const auto symTotal = getMaxOrd_() - zeroCtxUniqueCnt_.getTotalWordsCnt();
      ret.emplace_back(symLow, symHigh, symTotal);
    } else {
      const auto symLow = 2 * zeroCtxCnt_.getLowerCumulativeCount(ord) -
                          zeroCtxUniqueCnt_.getLowerCumulativeCount(ord);
      const auto symHigh = symLow + 2 * zeroCtxCnt_.getCount(ord) -
                           zeroCtxUniqueCnt_.getCount(ord);
      const auto symTotal = 2 * zeroCtxCnt_.getTotalWordsCnt();
      ret.emplace_back(symLow, symHigh, symTotal);
    }
    zeroCtxCnt_.increaseOrdCount(ord, 1);
    zeroCtxUniqueCnt_.update(ord);

    return ret;
  }
  const auto symLow =
      2 * ctxInfo_.at(currCtx).cnt.getLowerCumulativeCount(ord) -
      ctxInfo_.at(currCtx).uniqueCnt.getLowerCumulativeCount(ord);
  const auto symHigh = symLow + 2 * ctxInfo_.at(currCtx).cnt.getCount(ord) -
                       ctxInfo_.at(currCtx).uniqueCnt.getCount(ord);
  const auto symTotal = 2 * ctxInfo_.at(currCtx).cnt.getTotalWordsCnt();
  ret.emplace_back(symLow, symHigh, symTotal);
  for (; !currCtx.empty(); currCtx.pop_back()) {
    ctxInfo_.at(currCtx).cnt.increaseOrdCount(ord, 1);
    ctxInfo_.at(currCtx).uniqueCnt.update(1);
  }
  zeroCtxCnt_.increaseOrdCount(ord, 1);
  zeroCtxUniqueCnt_.update(ord);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getDecodeProbabilityStats(Ord ord) -> ProbabilityStats {
  auto ret = getDecodeProbabilityStats_(ord);
  if (!isEsc(ord)) {
    updateWordCnt_(ord, 1);
    updateCtx_(ord);
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getTotalWordsCnt() const -> Count {
  auto currCtx = SearchCtx_(ctx_.rbegin(), ctx_.rend());
  skipNewCtxs_(currCtx);
  if (getEscDecoded_() < currCtx.size()) {
    skipCtxsByEsc_(currCtx);
    return 2 * ctxInfo_.at(currCtx).cnt.getTotalWordsCnt();
  }
  if (getEscDecoded_() == currCtx.size()) {
    if (0 == zeroCtxCnt_.getTotalWordsCnt()) {
      return 1;
    }
    return 2 * zeroCtxCnt_.getTotalWordsCnt();
  }
  assert(getEscDecoded_() == currCtx.size() + 1 &&
         "Esc decode count can not be that big.");
  return getMaxOrd_() - zeroCtxUniqueCnt_.getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getDecodeProbabilityStats_(Ord ord) -> ProbabilityStats {
  auto currCtx = SearchCtx_(ctx_.rbegin(), ctx_.rend());
  skipNewCtxs_(currCtx);
  if (getEscDecoded_() >= currCtx.size()) {
    if (isEsc(ord)) {
      assert(getEscDecoded_() == currCtx.size() &&
             "escDecoded_ can not be greater than size at this moment.");
      updateEscDecoded_(ord);
      return getZeroCtxEscStats_();
    }
    if (getEscDecoded_() == currCtx.size()) {
      const auto symLow = 2 * zeroCtxCnt_.getLowerCumulativeCount(ord) -
                          zeroCtxUniqueCnt_.getLowerCumulativeCount(ord);
      const auto symHigh = symLow + zeroCtxCnt_.getCount(ord) -
                           2 * zeroCtxUniqueCnt_.getCount(ord);
      const auto symTotal = 2 * zeroCtxCnt_.getTotalWordsCnt();
      updateEscDecoded_(ord);
      assert(!isEsc(ord) && "ord can not be esc at this moment.");
      return getDecodeProbabilityStatsForNewWord_(ord);
    }
  }
  skipCtxsByEsc_(currCtx);
  updateEscDecoded_(ord);
  const auto& currCtxInfo = ctxInfo_.at(currCtx);
  if (isEsc(ord)) {
    const auto escLow = 2 * currCtxInfo.cnt.getTotalWordsCnt() -
                        currCtxInfo.uniqueCnt.getTotalWordsCnt();
    const auto escHigh = escLow + currCtxInfo.uniqueCnt.getTotalWordsCnt();
    const auto escTotal = 2 * currCtxInfo.cnt.getTotalWordsCnt();
    return {escLow, escHigh, escTotal};
  }
  const auto symLow = 2 * currCtxInfo.cnt.getLowerCumulativeCount(ord) -
                      currCtxInfo.uniqueCnt.getLowerCumulativeCount(ord);
  const auto symHigh = symLow + currCtxInfo.cnt.getCount(ord) -
                       currCtxInfo.uniqueCnt.getCount(ord);
  const auto symTotal = 2 * currCtxInfo.cnt.getTotalWordsCnt();
  return {symLow, symHigh, symTotal};
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getDecodeProbabilityStatsForNewWord_(Ord ord) const
    -> ProbabilityStats {
  const auto symLow =
      Count{ord} - zeroCtxUniqueCnt_.getLowerCumulativeCount(ord);
  const auto symHigh = symLow + 1;
  const auto symTotal = getMaxOrd_() - zeroCtxUniqueCnt_.getTotalWordsCnt();
  return {symLow, symHigh, symTotal};
}

////////////////////////////////////////////////////////////////////////////////
void PPMDDictionary::updateWordCnt_(Ord ord, std::int64_t cntChange) {
  auto currCtx = SearchCtx_(ctx_.rbegin(), ctx_.rend());
  for (; !currCtx.empty() && !ctxInfo_.contains(currCtx); currCtx.pop_back()) {
    ctxInfo_.emplace(currCtx,
                     CtxCell_{impl::dict::CumulativeCount(getMaxOrd_()),
                              impl::dict::CumulativeUniqueCount(getMaxOrd_())});
    ctxInfo_.at(currCtx).cnt.increaseOrdCount(ord, cntChange);
    ctxInfo_.at(currCtx).uniqueCnt.update(ord);
  }
  for (; !currCtx.empty(); currCtx.pop_back()) {
    ctxInfo_.at(currCtx).cnt.increaseOrdCount(ord, cntChange);
    ctxInfo_.at(currCtx).uniqueCnt.update(ord);
  }
  zeroCtxCnt_.increaseOrdCount(ord, cntChange);
  zeroCtxUniqueCnt_.update(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getWordOrdForNewWord_(Count cumulativeCnt) const -> Ord {
  const auto idxs = rng::iota_view(Ord{0}, getMaxOrd_());
  const auto getLowerCumulCnt = [this](Ord ord) {
    return ord + 1 - zeroCtxUniqueCnt_.getLowerCumulativeCount(ord + 1);
  };
  const auto retOrd =
      rng::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulCnt) -
      idxs.begin();
  assert(!isEsc(retOrd) &&
         "Search in symbols which were not found yet. Esc is invalid here.");
  return retOrd;
}

////////////////////////////////////////////////////////////////////////////////
void PPMDDictionary::updateCtx_(Ord ord) {
  ctx_.push_back(ord);
  if (ctx_.size() > ctxLength_) {
    ctx_.pop_front();
  }
}

////////////////////////////////////////////////////////////////////////////////
void PPMDDictionary::skipNewCtxs_(SearchCtx_& currCtx) const {
  for (; !currCtx.empty() && !ctxInfo_.contains(currCtx); currCtx.pop_back()) {
    // Skip all empty contexts.
  }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getZeroCtxEscStats_() const -> ProbabilityStats {
  const auto escLow =
      2 * zeroCtxCnt_.getTotalWordsCnt() - zeroCtxUniqueCnt_.getTotalWordsCnt();
  const auto escHigh = escLow + zeroCtxUniqueCnt_.getTotalWordsCnt();
  const auto escTotal = 2 * zeroCtxCnt_.getTotalWordsCnt();
  return {escLow, escHigh, escTotal};
}

}  // namespace ael::esc::dict
