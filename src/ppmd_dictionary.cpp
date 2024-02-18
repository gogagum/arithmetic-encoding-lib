#include <ael/dictionary/ppmd_dictionary.hpp>
#include <ael/impl/dictionary/cumulative_count.hpp>
#include <ael/impl/dictionary/cumulative_unique_count.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <ranges>
#include <stdexcept>

namespace ael::dict {

using boost::lambda::_1;
using boost::lambda::bind;
using std::ranges::upper_bound;

////////////////////////////////////////////////////////////////////////////////
PPMDDictionary::PPMDDictionary(ConstructInfo constructInfo)
    : Base_(constructInfo.maxOrd, constructInfo.ctxLength),
      zeroCtxCell_{constructInfo.maxOrd} {
  /**
   * \tau_{ctx}_{i} < sequenceLength
   * Product of tau-s must be less than sequenceLength ^ "tau-s count"
   * Estimation: sequenceLength * l_{ctx} < maxCntBits.
   */
  if (maxSeqLenLog2_ * getCtxLength_() > countNumBits) {
    throw std::logic_error("Too big context.");
  }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getWordOrd(const Count& cumulativeNumFound) const -> Ord {
  return *upper_bound(getOrdRng_(), cumulativeNumFound, {},
                      bind(&This_::getLowerCumulativeCnt_, this, _1 + 1));
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  auto ret = getProbabilityStats_(ord);
  updateWordCnt_(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getTotalWordsCnt() const -> Count {
  Count total = 1;
  for (auto ctx = getSearchCtxEmptySkipped_(); !ctx.empty(); ctx.pop_back()) {
    const auto totalCnt = ctxInfo_.at(ctx).cnt.getTotalWordsCnt();
    total *= totalCnt * 2;
  }
  if (const auto totalZeroCtxCnt = zeroCtxCell_.cnt.getTotalWordsCnt();
      totalZeroCtxCnt != 0) {
    total *= totalZeroCtxCnt * 2;
  }
  if (const auto zeroUniqueCnt = zeroCtxCell_.uniqueCnt.getTotalWordsCnt();
      zeroUniqueCnt < getMaxOrd_()) {
    total *= getMaxOrd_() - zeroUniqueCnt;
  }
  return total;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  assert(ord <= getMaxOrd_());
  Count lower = 0;
  Count uniqueCountsProd = 1;
  for (auto ctx = getSearchCtxEmptySkipped_(); !ctx.empty(); ctx.pop_back()) {
    const auto& ctxCell = ctxInfo_.at(ctx);
    const auto ctxTotalCnt = ctxCell.cnt.getTotalWordsCnt();
    lower *= ctxTotalCnt * 2;
    const auto lowerCnt = ctxCell.cnt.getLowerCumulativeCnt(ord);
    const auto lowerUniqueCnt = ctxCell.uniqueCnt.getLowerCumulativeCnt(ord);
    lower += (lowerCnt * 2 - lowerUniqueCnt) * uniqueCountsProd;
    uniqueCountsProd *= ctxCell.uniqueCnt.getTotalWordsCnt();
  }
  const auto zeroCtxTotalUniqueCnt = zeroCtxCell_.uniqueCnt.getTotalWordsCnt();
  if (const auto zeroCtxTotalCnt = zeroCtxCell_.cnt.getTotalWordsCnt();
      zeroCtxTotalCnt != 0) {
    lower *= zeroCtxTotalCnt * 2;
    const auto cumulativeCnt = zeroCtxCell_.cnt.getLowerCumulativeCnt(ord);
    const auto cumulativeUniqueCnt =
        zeroCtxCell_.uniqueCnt.getLowerCumulativeCnt(ord);
    lower += (cumulativeCnt * 2 - cumulativeUniqueCnt) * uniqueCountsProd;
    uniqueCountsProd *= zeroCtxTotalUniqueCnt;
  }
  if (zeroCtxTotalUniqueCnt < getMaxOrd_()) {
    lower *= getMaxOrd_() - zeroCtxTotalUniqueCnt;
    lower += (ord - zeroCtxCell_.uniqueCnt.getLowerCumulativeCnt(ord)) *
             uniqueCountsProd;
  }
  return {lower};
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getProbabilityStats_(Ord ord) const -> ProbabilityStats {
  assert(ord < getMaxOrd_());
  Count lower = 0;
  Count count = 0;
  Count total = 1;
  Count uniqueCountsProd = 1;
  for (auto ctx = getSearchCtxEmptySkipped_(); !ctx.empty(); ctx.pop_back()) {
    const auto& ctxInfo = ctxInfo_.at(ctx).cnt;
    const auto& ctxUniqueInfo = ctxInfo_.at(ctx).uniqueCnt;
    const auto ctxTotalCnt = ctxInfo.getTotalWordsCnt();
    lower *= ctxTotalCnt * 2;
    const auto lowerCnt = ctxInfo.getLowerCumulativeCnt(ord);
    const auto lowerUniqueCnt = ctxUniqueInfo.getLowerCumulativeCnt(ord);
    lower += (lowerCnt * 2 - lowerUniqueCnt) * uniqueCountsProd;
    total *= ctxTotalCnt * 2;
    count *= ctxTotalCnt * 2;
    const auto cnt = ctxInfo.getCount(ord);
    const auto uniqueCnt = ctxUniqueInfo.getCount(ord);
    count += (cnt * 2 - uniqueCnt) * uniqueCountsProd;
    uniqueCountsProd *= ctxUniqueInfo.getTotalWordsCnt();
  }
  const auto zeroCtxUniqueTotal = zeroCtxCell_.uniqueCnt.getTotalWordsCnt();
  const auto zeroCtxTotal = zeroCtxCell_.cnt.getTotalWordsCnt();
  lower *= zeroCtxTotal * 2;
  const auto lowerCnt = zeroCtxCell_.cnt.getLowerCumulativeCnt(ord);
  const auto lowerUniqueCnt = zeroCtxCell_.uniqueCnt.getLowerCumulativeCnt(ord);
  lower += (lowerCnt * 2 - lowerUniqueCnt) * uniqueCountsProd;
  count *= zeroCtxTotal * 2;
  const auto cnt = zeroCtxCell_.cnt.getCount(ord);
  const auto uniqueCnt = zeroCtxCell_.uniqueCnt.getCount(ord);
  count += (cnt * 2 - uniqueCnt) * uniqueCountsProd;
  if (zeroCtxTotal != 0) {
    total *= zeroCtxTotal * 2;
    uniqueCountsProd *= zeroCtxUniqueTotal;
  }
  if (zeroCtxUniqueTotal < getMaxOrd_()) {
    total *= getMaxOrd_() - zeroCtxUniqueTotal;
    lower *= getMaxOrd_() - zeroCtxUniqueTotal;
    lower += (ord - zeroCtxCell_.uniqueCnt.getLowerCumulativeCnt(ord)) *
             uniqueCountsProd;
    count *= getMaxOrd_() - zeroCtxUniqueTotal;
    count += uniqueCountsProd * (1 - zeroCtxCell_.uniqueCnt.getCount(ord));
  }
  assert(count > 0);
  assert(lower + count <= total);
  return {lower, lower + count, total};
}

////////////////////////////////////////////////////////////////////////////////
void PPMDDictionary::updateWordCnt_(Ord ord, std::int64_t cnt) {
  for (auto ctx = getInitSearchCtx_(); !ctx.empty(); ctx.pop_back()) {
    if (!ctxInfo_.contains(ctx)) {
      ctxInfo_.emplace(ctx, CtxCell_(getMaxOrd_()));
    }
    ctxInfo_.at(ctx).cnt.increaseOrdCount(ord, cnt);
    ctxInfo_.at(ctx).uniqueCnt.update(ord);
  }
  zeroCtxCell_.cnt.increaseOrdCount(ord, cnt);
  zeroCtxCell_.uniqueCnt.update(ord);
  updateCtx_(ord);
}

}  // namespace ael::dict
