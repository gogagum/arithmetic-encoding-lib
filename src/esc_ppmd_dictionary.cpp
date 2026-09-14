#include <ael/esc/dictionary/ppmd_dictionary.hpp>
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
PPMDDictionary::PPMDDictionary(ConstructInfo constructInfo)
    : ael::impl::esc::dict::PPMADDictionaryBase<PPMDDictionary>(
          constructInfo.maxOrd, constructInfo.ctxLength),
      zeroCtxCell_(constructInfo.maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  auto currCtx = getSearchCtxEmptySkipped_();
  if (0 == zeroCtxCell_.cnt.getTotalWordsCnt() &&
      getEscDecoded_() == currCtx.size()) [[unlikely]] {
    return getMaxOrd_();
  }
  if (getEscDecoded_() <= currCtx.size()) {
    const auto& cell = getCurrCtxCell_(currCtx);
    return *upper_bound(
        getOrdRng_(), cumulativeCnt, {},
        2 * bind(&CumulativeCount_::getLowerCumulativeCnt, &cell.cnt, _1 + 1) -
            bind(&CumulativeUniqueCount_::getLowerCumulativeCnt,
                 &cell.uniqueCnt, _1 + 1));
  }
  return getWordOrdForNewWord_(cumulativeCnt);
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getProbabilityStats(Ord ord) -> StatsSeq {
  StatsSeq ret;
  SearchCtx_ currCtx = getInitSearchCtx_();
  updateCtx_(ord);
  for (; !currCtx.empty() && !ctxInfo_.contains(currCtx); currCtx.pop_back()) {
    auto [iter, insertionHappened] = ctxInfo_.emplace(currCtx, getMaxOrd_());
    assert(insertionHappened && "Insertion must happen.");
    iter->second.cnt.increaseOrdCount(ord, 1);
    iter->second.uniqueCnt.update(ord);
  }
  for (; !currCtx.empty(); currCtx.pop_back()) {
    auto& currCtxInfo = ctxInfo_.at(currCtx);
    if (currCtxInfo.cnt.getCount(ord) > 0) {
      break;
    }
    const auto currTotal = currCtxInfo.cnt.getTotalWordsCnt();
    const auto currTotalUnique = currCtxInfo.uniqueCnt.getTotalWordsCnt();
    const auto escLow = (2 * currTotal) - currTotalUnique;
    const auto escHigh = escLow + currTotalUnique;
    const auto escTotal = 2 * currTotal;
    ret.emplace_back(escLow, escHigh, escTotal);
    currCtxInfo.cnt.increaseOrdCount(ord, 1);
    currCtxInfo.uniqueCnt.update(ord);
  }
  if (currCtx.empty()) {
    const auto zeroTotal = zeroCtxCell_.cnt.getTotalWordsCnt();
    const auto zeroLowerUnique =
        zeroCtxCell_.uniqueCnt.getLowerCumulativeCnt(ord);
    if (const auto zeroCount = zeroCtxCell_.cnt.getCount(ord); 0 == zeroCount) {
      const auto zeroTotalUnique = zeroCtxCell_.uniqueCnt.getTotalWordsCnt();
      const auto escLow = (2 * zeroTotal) - zeroTotalUnique;
      const auto escHigh = escLow + std::max(Count{1}, zeroTotalUnique);
      const auto escTotal = std::max(Count{1}, 2 * zeroTotal);
      ret.emplace_back(escLow, escHigh, escTotal);
      const auto symLow = Count{ord} - zeroLowerUnique;
      const auto symHigh = symLow + 1;
      const auto symTotal = getMaxOrd_() - zeroTotalUnique;
      ret.emplace_back(symLow, symHigh, symTotal);
    } else {
      const auto zeroLower = zeroCtxCell_.cnt.getLowerCumulativeCnt(ord);
      const auto zeroUnique = zeroCtxCell_.uniqueCnt.getCount(ord);
      const auto symLow = (2 * zeroLower) - zeroLowerUnique;
      const auto symHigh = symLow + (2 * zeroCount) - zeroUnique;
      const auto symTotal = 2 * zeroTotal;
      ret.emplace_back(symLow, symHigh, symTotal);
    }
  } else {
    const auto& currCtxInfo = ctxInfo_.at(currCtx);
    const auto lowerCnt = currCtxInfo.cnt.getLowerCumulativeCnt(ord);
    const auto lowerUniqueCnt =
        currCtxInfo.uniqueCnt.getLowerCumulativeCnt(ord);
    const auto symLow = (2 * lowerCnt) - lowerUniqueCnt;
    const auto cnt = currCtxInfo.cnt.getCount(ord);
    const auto uniqueCnt = currCtxInfo.uniqueCnt.getCount(ord);
    const auto symHigh = symLow + (2 * cnt) - uniqueCnt;
    const auto symTotal = 2 * currCtxInfo.cnt.getTotalWordsCnt();
    ret.emplace_back(symLow, symHigh, symTotal);
    for (; !currCtx.empty(); currCtx.pop_back()) {
      auto& currCtxInfo = ctxInfo_.at(currCtx);
      currCtxInfo.cnt.increaseOrdCount(ord, 1);
      currCtxInfo.uniqueCnt.update(ord);
    }
  }
  zeroCtxCell_.cnt.increaseOrdCount(ord, 1);
  zeroCtxCell_.uniqueCnt.update(ord);
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
  auto currCtx = getSearchCtxEmptySkipped_();
  if (getEscDecoded_() < currCtx.size()) {
    skipCtxsByEsc_(currCtx);
    return 2 * ctxInfo_.at(currCtx).cnt.getTotalWordsCnt();
  }
  if (getEscDecoded_() == currCtx.size()) {
    return std::max(Count{1}, 2 * zeroCtxCell_.cnt.getTotalWordsCnt());
  }
  assert(getEscDecoded_() == currCtx.size() + 1 &&
         "Esc decode count can not be that big.");
  return getMaxOrd_() - zeroCtxCell_.uniqueCnt.getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getDecodeProbabilityStats_(const Ord ord)
    -> ProbabilityStats {
  const auto ord_update = [ord](PPMDDictionary* const val) {
    val->updateEscDecoded_(ord);
  };
  std::unique_ptr<PPMDDictionary, decltype(ord_update)> esc_decoded_update{
      this, ord_update};
  SearchCtx_ currCtx = getSearchCtxEmptySkipped_();
  if (getEscDecoded_() >= currCtx.size()) {
    if (isEsc(ord)) {
      assert(
          getEscDecoded_() == currCtx.size() &&
          "escDecoded_ can not be greater than context size at this moment.");
      return getZeroCtxEscStats_();
    }
    if (getEscDecoded_() == currCtx.size()) {
      if (0 == zeroCtxCell_.cnt.getTotalWordsCnt()) {
        return {
            .low = 0,
            .high = 1,
            .total = 1,
        };
      }
      const Count zeroLower = zeroCtxCell_.cnt.getLowerCumulativeCnt(ord);
      const Count zeroLowerUnique =
          zeroCtxCell_.uniqueCnt.getLowerCumulativeCnt(ord);
      const Count zeroCnt = zeroCtxCell_.cnt.getCount(ord);
      const Count zeroUniqueCnt = zeroCtxCell_.uniqueCnt.getCount(ord);
      const Count symLow = (2 * zeroLower) - zeroLowerUnique;
      return {
          .low = symLow,
          .high = symLow + (2 * zeroCnt) - zeroUniqueCnt,
          .total = 2 * zeroCtxCell_.cnt.getTotalWordsCnt(),
      };
    }
    assert(getEscDecoded_() == currCtx.size() + 1 &&
           "escDecoded_ can not be that big.");
    const Count symLow =
        Count{ord} - zeroCtxCell_.uniqueCnt.getLowerCumulativeCnt(ord);
    return {
        .low = symLow,
        .high = symLow + 1,
        .total = getMaxOrd_() - zeroCtxCell_.uniqueCnt.getTotalWordsCnt(),
    };
  }
  skipCtxsByEsc_(currCtx);
  const CtxCell_& currCtxInfo = ctxInfo_.at(currCtx);
  const Count totalCnt = currCtxInfo.cnt.getTotalWordsCnt();
  if (isEsc(ord)) {
    return {
        .low = (2 * totalCnt) - currCtxInfo.uniqueCnt.getTotalWordsCnt(),
        .high = 2 * totalCnt,
        .total = 2 * totalCnt,
    };
  }
  const Count symLow = (2 * currCtxInfo.cnt.getLowerCumulativeCnt(ord)) -
                       currCtxInfo.uniqueCnt.getLowerCumulativeCnt(ord);
  return {
      .low = symLow,
      .high = symLow + (2 * currCtxInfo.cnt.getCount(ord)) -
              currCtxInfo.uniqueCnt.getCount(ord),
      .total = 2 * totalCnt,
  };
}

////////////////////////////////////////////////////////////////////////////////
void PPMDDictionary::updateWordCnt_(Ord ord, std::int64_t cntChange) {
  SearchCtx_ currCtx = getInitSearchCtx_();
  for (; !currCtx.empty() && !ctxInfo_.contains(currCtx); currCtx.pop_back()) {
    auto [iter, insertionHappened] = ctxInfo_.emplace(currCtx, getMaxOrd_());
    assert(insertionHappened && "Insertion must happen.");
    iter->second.cnt.increaseOrdCount(ord, cntChange);
    iter->second.uniqueCnt.update(ord);
  }
  for (; !currCtx.empty(); currCtx.pop_back()) {
    auto& currCtxInfo = ctxInfo_.at(currCtx);
    currCtxInfo.cnt.increaseOrdCount(ord, cntChange);
    currCtxInfo.uniqueCnt.update(ord);
  }
  zeroCtxCell_.cnt.increaseOrdCount(ord, cntChange);
  zeroCtxCell_.uniqueCnt.update(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getWordOrdForNewWord_(Count cumulativeCnt) const -> Ord {
  const auto retOrd =
      *upper_bound(getOrdRng_(), cumulativeCnt, {},
                   _1 + 1 -
                       bind(&CumulativeUniqueCount_::getLowerCumulativeCnt,
                            &zeroCtxCell_.uniqueCnt, _1 + 1));
  assert(!isEsc(retOrd) &&
         "Search in symbols which were not found yet. Esc is invalid here.");
  return retOrd;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getZeroCtxEscStats_() const -> ProbabilityStats {
  const auto zeroTotal = zeroCtxCell_.cnt.getTotalWordsCnt();
  if (0 == zeroTotal) [[unlikely]] {
    return {0, 1, 1};
  }
  const auto escLow =
      (2 * zeroTotal) - zeroCtxCell_.uniqueCnt.getTotalWordsCnt();
  const auto escHigh = escLow + zeroCtxCell_.uniqueCnt.getTotalWordsCnt();
  const auto escTotal = 2 * zeroCtxCell_.cnt.getTotalWordsCnt();
  return {escLow, escHigh, escTotal};
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getCurrCtxCell_(SearchCtx_& currCtx) const
    -> const CtxCell_& {
  if (getEscDecoded_() < currCtx.size()) {
    skipCtxsByEsc_(currCtx);
    return ctxInfo_.at(currCtx);
  }
  assert(getEscDecoded_() == currCtx.size());
  return zeroCtxCell_;
}

}  // namespace ael::esc::dict
