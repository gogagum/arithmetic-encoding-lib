#include <ael/esc/dictionary/ppma_dictionary.hpp>
#include <ael/impl/dictionary/cumulative_count.hpp>
#include <algorithm>
#include <cstddef>
#include <ranges>
#include <utility>

namespace ael::esc::dict {

////////////////////////////////////////////////////////////////////////////////
PPMADictionary::PPMADictionary(ConstructInfo constructInfo)
    : ael::impl::esc::dict::PPMADDictionaryBase(constructInfo.maxOrd),
      zeroCtxCnt_(constructInfo.maxOrd),
      zeroCtxUniqueCnt_(constructInfo.maxOrd),
      ctxLength_(constructInfo.ctxLength) {
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  const auto idxs = std::ranges::iota_view(Ord{0}, getMaxOrd_());
  if (escDecoded_ < ctx_.size()) {
    const auto ctxLength = ctx_.size() - escDecoded_;
    const auto currCtx = SearchCtx_(
        ctx_.rbegin(), ctx_.rbegin() + static_cast<std::ptrdiff_t>(ctxLength));
    const auto& currCtxInfo = ctxInfo_.at(currCtx);
    const auto getLowerCumulCnt = [&currCtxInfo](Ord ord) {
      return currCtxInfo.getLowerCumulativeCount(ord + 1);
    };
    const auto iter =
        std::ranges::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulCnt);
    const Ord retOrd = iter - idxs.begin();
    if (retOrd < getMaxOrd_()) {
      escDecoded_ = 0;
    }
    return retOrd;
  }
  if (escDecoded_ == ctx_.size()) {
    if (cumulativeCnt == zeroCtxCnt_.getTotalWordsCnt()) {
      ++escDecoded_;
      return getMaxOrd_();
    }
    const auto getLowerCumulCnt = [this](Ord ord) {
      return zeroCtxCnt_.getLowerCumulativeCount(ord + 1);
    };
    const auto iter =
        std::ranges::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulCnt);
    return iter - idxs.begin();
  }
  assert(escDecoded_ == ctx_.size() + 1 &&
         "Esc decoded count can not be that big.");
  const auto getLowerCumulCnt = [this](Ord ord) {
    return ord + 1 - zeroCtxUniqueCnt_.getLowerCumulativeCount(ord + 1);
  };
  const auto iter =
      std::ranges::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulCnt);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getProbabilityStats(Ord ord) -> StatsSeq {
  StatsSeq ret;
  auto currCtx = SearchCtx_(ctx_.rbegin(), ctx_.rend());
  for (; !currCtx.empty() && !ctxInfo_.contains(currCtx); currCtx.pop_back()) {
    ctxInfo_.emplace(currCtx, getMaxOrd_());
    ctxInfo_.at(currCtx).increaseOrdCount(ord, 1);
  }
  for (; !currCtx.empty() && ctxInfo_.at(currCtx).getCount(ord) == 0;
       currCtx.pop_back()) {
    const auto escLow = ctxInfo_.at(currCtx).getTotalWordsCnt();
    const auto escHigh = escLow + 1;
    const auto escTotal = ctxInfo_.at(currCtx).getTotalWordsCnt() + 1;
    ret.emplace_back(escLow, escHigh, escTotal);
    ctxInfo_.at(currCtx).increaseOrdCount(ord, 1);
  }
  if (currCtx.empty()) {
    if (0 == zeroCtxCnt_.getCount(ord)) {
      const auto escLow = zeroCtxCnt_.getTotalWordsCnt();
      const auto escHigh = escLow + 1;
      const auto escTotal = zeroCtxCnt_.getTotalWordsCnt() + 1;
      ret.emplace_back(escLow, escHigh, escTotal);
      const auto symLow =
          Count{ord} - zeroCtxUniqueCnt_.getLowerCumulativeCount(ord);
      const auto symHigh = symLow + 1;
      const auto symTotal = getMaxOrd_() - zeroCtxUniqueCnt_.getTotalWordsCnt();
      ret.emplace_back(symLow, symHigh, symTotal);
    } else {
      const auto symLow = zeroCtxCnt_.getLowerCumulativeCount(ord);
      const auto symHigh = symLow + zeroCtxCnt_.getCount(ord);
      const auto symTotal = zeroCtxCnt_.getTotalWordsCnt() + 1;
      ret.emplace_back(symLow, symHigh, symTotal);
    }

    zeroCtxCnt_.increaseOrdCount(ord, 1);
    zeroCtxUniqueCnt_.update(ord);

    return ret;
  }
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
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getTotalWordsCnt() const -> Count {
  if (escDecoded_ < ctx_.size()) {
    const auto currCtxLength = ctx_.size() - escDecoded_;
    const auto currCtx = SearchCtx_(
        ctx_.rbegin(), ctx_.rbegin() + static_cast<ptrdiff_t>(currCtxLength));
    return ctxInfo_.at(currCtx).getTotalWordsCnt() + 1;
  }
  if (escDecoded_ == ctx_.size()) {
    return zeroCtxCnt_.getTotalWordsCnt() + 1;
  }
  assert(escDecoded_ == ctx_.size() + 1 &&
         "Esc decode count can not be that big.");
  return getMaxOrd_() - zeroCtxUniqueCnt_.getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getDecodeProbabilityStats_(Ord ord) const
    -> ProbabilityStats {
  if (escDecoded_ >= ctx_.size()) {
    if (isEsc(ord)) {
      const auto escLow = zeroCtxCnt_.getTotalWordsCnt();
      const auto escHigh = escLow + 1;
      const auto escTotal = zeroCtxCnt_.getTotalWordsCnt() + 1;
      return {escLow, escHigh, escTotal};
    }
    if (escDecoded_ == ctx_.size()) {
      escDecoded_ = 0;
      const auto symLow = zeroCtxCnt_.getLowerCumulativeCount(ord);
      const auto symHigh = symLow + zeroCtxCnt_.getCount(ord);
      const auto symTotal = zeroCtxCnt_.getTotalWordsCnt() + 1;
      return {symLow, symHigh, symTotal};
    }
    return getDecodeProbabilityStatsForNewWord_(ord);
  }
  const auto ctxLength = ctx_.size() - escDecoded_;
  const auto currCtx = getSearchCtxOfLength_(ctxLength);
  const auto& currCtxInfo = ctxInfo_.at(currCtx);
  if (isEsc(ord)) {
    const auto escLow = currCtxInfo.getTotalWordsCnt();
    const auto escHigh = escLow + 1;
    const auto escTotal = currCtxInfo.getTotalWordsCnt() + 1;
    return {escLow, escHigh, escTotal};
  } else {
    escDecoded_ = 0;
    const auto symLow = currCtxInfo.getCumulativeCount(ord);
    const auto symHigh = symLow + currCtxInfo.getCount(ord);
    const auto symTotal = currCtxInfo.getTotalWordsCnt() + 1;
    return {symLow, symHigh, symTotal};
  }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getDecodeProbabilityStatsForNewWord_(Ord ord) const
    -> ProbabilityStats {
  assert(escDecoded_ == ctx_.size() + 1 && "escDecoded_ can not be that big.");
  escDecoded_ = 0;
  const auto symLow = Count{ord} - zeroCtxUniqueCnt_.getCumulativeCount(ord);
  const auto symHigh = symLow + 1;
  const auto symTotal = getMaxOrd_() - zeroCtxUniqueCnt_.getTotalWordsCnt();
  return {symLow, symHigh, symTotal};
}

////////////////////////////////////////////////////////////////////////////////
void PPMADictionary::updateWordCnt_(Ord ord, std::int64_t cntChange) {
  if (escDecoded_ >= ctx_.size()) {
    zeroCtxCnt_.increaseOrdCount(ord, cntChange);
    zeroCtxUniqueCnt_.update(ord);
  }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getSearchCtxOfLength_(std::size_t ctxLength) const
    -> SearchCtx_ {
  return {ctx_.rbegin(), ctx_.rbegin() + static_cast<ptrdiff_t>(ctxLength)};
}

}  // namespace ael::esc::dict
