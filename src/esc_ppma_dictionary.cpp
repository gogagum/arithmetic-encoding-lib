#include <ael/esc/dictionary/ppma_dictionary.hpp>
#include <algorithm>
#include <ranges>

namespace ael::esc::dict {

namespace rng = std::ranges;

////////////////////////////////////////////////////////////////////////////////
PPMADictionary::PPMADictionary(ConstructInfo constructInfo)
    : ael::impl::esc::dict::PPMADDictionaryBase(constructInfo.maxOrd,
                                                constructInfo.ctxLength),
      zeroCtxCnt_(constructInfo.maxOrd),
      zeroCtxUniqueCnt_(constructInfo.maxOrd),
      ctxLength_(constructInfo.ctxLength) {
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  const auto idxs = rng::iota_view(Ord{0}, getMaxOrd_());
  auto currCtx = SearchCtx_(getCtx_().rbegin(), getCtx_().rend());
  skipNewCtxs_(currCtx);
  if (getEscDecoded_() < currCtx.size()) {
    skipCtxsByEsc_(currCtx);
    const auto& currCtxInfo = ctxInfo_.at(currCtx);
    const auto getLowerCumulCnt = [&currCtxInfo](Ord ord) {
      return currCtxInfo.getLowerCumulativeCnt(ord + 1);
    };
    return rng::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulCnt) -
           idxs.begin();
  }
  if (getEscDecoded_() == currCtx.size()) {
    const auto getLowerCumulCnt = [this](Ord ord) {
      return zeroCtxCnt_.getLowerCumulativeCnt(ord + 1);
    };
    return rng::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulCnt) -
           idxs.begin();
  }
  assert(getEscDecoded_() == currCtx.size() + 1 &&
         "Esc decoded count can not be that big.");
  return getWordOrdForNewWord_(cumulativeCnt);
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getProbabilityStats(Ord ord) -> StatsSeq {
  StatsSeq ret;
  auto currCtx = SearchCtx_(getCtx_().rbegin(), getCtx_().rend());
  updateCtx_(ord);  // ctx_ is never read later
  for (; !currCtx.empty() && !ctxInfo_.contains(currCtx); currCtx.pop_back()) {
    ctxInfo_.emplace(currCtx, getMaxOrd_());
    ctxInfo_.at(currCtx).increaseOrdCount(ord, 1);
  }
  for (; !currCtx.empty() && ctxInfo_.at(currCtx).getCount(ord) == 0;
       currCtx.pop_back()) {
    const auto totalCnt = ctxInfo_.at(currCtx).getTotalWordsCnt();
    const auto escLow = totalCnt;
    const auto escHigh = escLow + 1;
    const auto escTotal = totalCnt + 1;
    ret.emplace_back(escLow, escHigh, escTotal);
    ctxInfo_.at(currCtx).increaseOrdCount(ord, 1);
  }
  if (currCtx.empty()) {
    const auto zeroTotal = zeroCtxCnt_.getTotalWordsCnt();
    if (const auto zeroCnt = zeroCtxCnt_.getCount(ord); 0 == zeroCnt) {
      const auto escLow = zeroTotal;
      const auto escHigh = escLow + 1;
      const auto escTotal = zeroTotal + 1;
      ret.emplace_back(escLow, escHigh, escTotal);
      const auto zeroLower = zeroCtxUniqueCnt_.getLowerCumulativeCnt(ord);
      const auto symLow = Count{ord} - zeroLower;
      const auto symHigh = symLow + 1;
      const auto symTotal = getMaxOrd_() - zeroCtxUniqueCnt_.getTotalWordsCnt();
      ret.emplace_back(symLow, symHigh, symTotal);
    } else {
      const auto symLow = zeroCtxCnt_.getLowerCumulativeCnt(ord);
      const auto symHigh = symLow + zeroCnt;
      const auto symTotal = zeroTotal + 1;
      ret.emplace_back(symLow, symHigh, symTotal);
    }

    zeroCtxCnt_.increaseOrdCount(ord, 1);
    zeroCtxUniqueCnt_.update(ord);

    return ret;
  }
  const auto& currCtxInfo = ctxInfo_.at(currCtx);
  const auto symLow = currCtxInfo.getLowerCumulativeCnt(ord);
  const auto symHigh = symLow + currCtxInfo.getCount(ord);
  const auto symTotal = currCtxInfo.getTotalWordsCnt() + 1;
  ret.emplace_back(symLow, symHigh, symTotal);
  for (; !currCtx.empty(); currCtx.pop_back()) {
    ctxInfo_.at(currCtx).increaseOrdCount(ord, 1);
  }
  zeroCtxCnt_.increaseOrdCount(ord, 1);
  zeroCtxUniqueCnt_.update(ord);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getDecodeProbabilityStats(Ord ord) -> ProbabilityStats {
  auto ret = getDecodeProbabilityStats_(ord);
  if (!isEsc(ord)) {
    updateWordCnt_(ord, 1);
    updateCtx_(ord);
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getTotalWordsCnt() const -> Count {
  auto currCtx = SearchCtx_(getCtx_().rbegin(), getCtx_().rend());
  skipNewCtxs_(currCtx);
  if (getEscDecoded_() < currCtx.size()) {
    skipCtxsByEsc_(currCtx);
    return ctxInfo_.at(currCtx).getTotalWordsCnt() + 1;
  }
  if (getEscDecoded_() == currCtx.size()) {
    return zeroCtxCnt_.getTotalWordsCnt() + 1;
  }
  assert(getEscDecoded_() == currCtx.size() + 1 &&
         "Esc decode count can not be that big.");
  return getMaxOrd_() - zeroCtxUniqueCnt_.getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getDecodeProbabilityStats_(Ord ord) -> ProbabilityStats {
  auto currCtx = SearchCtx_(getCtx_().rbegin(), getCtx_().rend());
  skipNewCtxs_(currCtx);
  if (getEscDecoded_() >= currCtx.size()) {
    if (isEsc(ord)) {
      assert(
          getEscDecoded_() == currCtx.size() &&
          "escDecoded_ can not be greater than context size at this moment.");
      updateEscDecoded_(ord);
      return getZeroCtxEscStats_();
    }
    if (getEscDecoded_() == currCtx.size()) {
      const auto symLow = zeroCtxCnt_.getLowerCumulativeCnt(ord);
      const auto symHigh = symLow + zeroCtxCnt_.getCount(ord);
      const auto symTotal = zeroCtxCnt_.getTotalWordsCnt() + 1;
      updateEscDecoded_(ord);
      return {symLow, symHigh, symTotal};
    }
    assert(getEscDecoded_() == currCtx.size() + 1 &&
           "escDecoded_ can not be that big.");
    updateEscDecoded_(ord);
    assert(!isEsc(ord) && "ord can not be esc at this moment.");
    return getDecodeProbabilityStatsForNewWord_(ord);
  }
  skipCtxsByEsc_(currCtx);
  updateEscDecoded_(ord);
  const auto& currCtxInfo = ctxInfo_.at(currCtx);
  if (isEsc(ord)) {
    const auto escLow = currCtxInfo.getTotalWordsCnt();
    const auto escHigh = escLow + 1;
    const auto escTotal = currCtxInfo.getTotalWordsCnt() + 1;
    return {escLow, escHigh, escTotal};
  }
  const auto symLow = currCtxInfo.getLowerCumulativeCnt(ord);
  const auto symHigh = symLow + currCtxInfo.getCount(ord);
  const auto symTotal = currCtxInfo.getTotalWordsCnt() + 1;
  return {symLow, symHigh, symTotal};
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getDecodeProbabilityStatsForNewWord_(Ord ord) const
    -> ProbabilityStats {
  const auto symLow = Count{ord} - zeroCtxUniqueCnt_.getLowerCumulativeCnt(ord);
  const auto symHigh = symLow + 1;
  const auto symTotal = getMaxOrd_() - zeroCtxUniqueCnt_.getTotalWordsCnt();
  return {symLow, symHigh, symTotal};
}

////////////////////////////////////////////////////////////////////////////////
void PPMADictionary::updateWordCnt_(Ord ord, std::int64_t cntChange) {
  auto currCtx = SearchCtx_(getCtx_().rbegin(), getCtx_().rend());
  for (; !currCtx.empty() && !ctxInfo_.contains(currCtx); currCtx.pop_back()) {
    auto [iter, insertionHappened] = ctxInfo_.emplace(currCtx, getMaxOrd_());
    assert(insertionHappened && "Insertion must has happened here.");
    iter->second.increaseOrdCount(ord, cntChange);
  }
  for (; !currCtx.empty(); currCtx.pop_back()) {
    ctxInfo_.at(currCtx).increaseOrdCount(ord, cntChange);
  }
  zeroCtxCnt_.increaseOrdCount(ord, cntChange);
  zeroCtxUniqueCnt_.update(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getWordOrdForNewWord_(Count cumulativeCnt) const -> Ord {
  const auto idxs = rng::iota_view(Ord{0}, getMaxOrd_());
  const auto getLowerCumulCnt = [this](Ord ord) {
    return ord + 1 - zeroCtxUniqueCnt_.getLowerCumulativeCnt(ord + 1);
  };
  const auto retOrd =
      rng::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulCnt) -
      idxs.begin();
  assert(!isEsc(retOrd) &&
         "Search in symbols which were not found yet. Esc is invalid here.");
  return retOrd;
}

////////////////////////////////////////////////////////////////////////////////
void PPMADictionary::skipNewCtxs_(SearchCtx_& currCtx) const {
  for (; !currCtx.empty() && !ctxInfo_.contains(currCtx); currCtx.pop_back()) {
    // Skip all empty contexts.
  }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getZeroCtxEscStats_() const -> ProbabilityStats {
  const auto escLow = zeroCtxCnt_.getTotalWordsCnt();
  const auto escHigh = escLow + 1;
  const auto escTotal = zeroCtxCnt_.getTotalWordsCnt() + 1;
  return {escLow, escHigh, escTotal};
}

}  // namespace ael::esc::dict
