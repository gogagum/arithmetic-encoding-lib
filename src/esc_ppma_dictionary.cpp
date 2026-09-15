#include <ael/esc/dictionary/ppma_dictionary.hpp>
#include <algorithm>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <memory>
#include <ranges>

namespace ael::esc::dict {

using boost::lambda::_1;
using boost::lambda::bind;
using std::ranges::upper_bound;

////////////////////////////////////////////////////////////////////////////////
PPMADictionary::PPMADictionary(ConstructInfo constructInfo)
    : ael::impl::esc::dict::PPMADDictionaryBase<PPMADictionary>(
          constructInfo.maxOrd, constructInfo.ctxLength),
      zeroCtxCnt_(constructInfo.maxOrd),
      zeroCtxUniqueCnt_(constructInfo.maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  SearchCtx_ currCtx = getSearchCtxEmptySkipped_();
  if (getEscDecoded_() <= currCtx.size()) {
    const CumulativeCount_& cell = getCurrCumulativeCnt_(currCtx);
    return *upper_bound(
        getOrdRng_(), cumulativeCnt, {},
        bind(&CumulativeCount_::getLowerCumulativeCnt, &cell, _1 + 1));
  }
  return getWordOrdForNewWord_(cumulativeCnt);
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getProbabilityStats(Ord ord) -> StatsSeq {
  StatsSeq ret;
  SearchCtx_ currCtx = getInitSearchCtx_();
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
      const auto symLow =
          Count{ord} - zeroCtxUniqueCnt_.getLowerCumulativeCnt(ord);
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
  const std::unique_ptr ord_update_guard =
      ordUpdateGuard([ord](This_* const val) {
        val->updateProbabilityStats_(ord);
      });
  return getDecodeProbabilityStats_(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getTotalWordsCnt() const -> Count {
  auto currCtx = getSearchCtxEmptySkipped_();
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
auto PPMADictionary::getDecodeProbabilityStats_(const Ord ord)
    -> ProbabilityStats {
  const std::unique_ptr ord_update_guard =
      ordUpdateGuard([ord](This_* const val) {
        val->updateEscDecoded_(ord);
      });
  SearchCtx_ currCtx = getSearchCtxEmptySkipped_();
  if (getEscDecoded_() >= currCtx.size()) {
    if (isEsc(ord)) {
      assert(
          getEscDecoded_() == currCtx.size() &&
          "escDecoded_ can not be greater than context size at this moment.");
      const auto escLow = zeroCtxCnt_.getTotalWordsCnt();
      const auto escHigh = escLow + 1;
      return {
          .low = escLow,
          .high = escLow + 1,
          .total = escLow + 1,
      };
    }
    if (getEscDecoded_() == currCtx.size()) {
      const Count symLow = zeroCtxCnt_.getLowerCumulativeCnt(ord);
      return {
          .low = symLow,
          .high = symLow + zeroCtxCnt_.getCount(ord),
          .total = zeroCtxCnt_.getTotalWordsCnt() + 1,
      };
    }
    assert(getEscDecoded_() == currCtx.size() + 1 &&
           "escDecoded_ can not be that big.");
    const Count symLow =
        Count{ord} - zeroCtxUniqueCnt_.getLowerCumulativeCnt(ord);
    return {
        .low = symLow,
        .high = symLow + 1,
        .total = getMaxOrd_() - zeroCtxUniqueCnt_.getTotalWordsCnt(),
    };
  }
  skipCtxsByEsc_(currCtx);
  const CumulativeCount_& currCtxInfo = ctxInfo_.at(currCtx);
  if (isEsc(ord)) {
    const Count escLow = currCtxInfo.getTotalWordsCnt();
    return {
        .low = escLow,
        .high = escLow + 1,
        .total = currCtxInfo.getTotalWordsCnt() + 1,
    };
  }
  const Count symLow = currCtxInfo.getLowerCumulativeCnt(ord);
  return {
      .low = symLow,
      .high = symLow + currCtxInfo.getCount(ord),
      .total = currCtxInfo.getTotalWordsCnt() + 1,
  };
}

////////////////////////////////////////////////////////////////////////////////
void PPMADictionary::updateWordCnt_(Ord ord, std::int64_t cntChange) {
  SearchCtx_ currCtx = getInitSearchCtx_();
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
  const auto getLowerCumulCnt = [this](const Ord ord) {
    return ord - zeroCtxUniqueCnt_.getLowerCumulativeCnt(ord);
  };
  const Ord retOrd = *upper_bound(getOrdRng_(), cumulativeCnt, {},
                                  bind(getLowerCumulCnt, _1 + 1));
  assert(!isEsc(retOrd) &&
         "Search in symbols which were not found yet. Esc is invalid here.");
  return retOrd;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getCurrCumulativeCnt_(SearchCtx_& currCtx) const
    -> const CumulativeCount_& {
  if (getEscDecoded_() < currCtx.size()) {
    skipCtxsByEsc_(currCtx);
    return ctxInfo_.at(currCtx);
  }
  assert(getEscDecoded_() == currCtx.size());
  return zeroCtxCnt_;
}

}  // namespace ael::esc::dict
