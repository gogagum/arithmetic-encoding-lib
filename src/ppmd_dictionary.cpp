#include <ael/dictionary/ppmd_dictionary.hpp>
#include <ael/impl/dictionary/cumulative_count.hpp>
#include <ael/impl/dictionary/cumulative_unique_count.hpp>
#include <algorithm>
#include <ranges>
#include <stdexcept>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
PPMDDictionary::PPMDDictionary(ConstructInfo constructInfo)
    : maxOrd_(constructInfo.maxOrd),
      zeroCtxCell_{
          ael::impl::dict::CumulativeCount(constructInfo.maxOrd),
          ael::impl::dict::CumulativeUniqueCount(constructInfo.maxOrd)},
      ctxLength_(constructInfo.ctxLength) {
  /**
   * \tau_{ctx}_{i} < sequenceLength
   * Product of tau-s must be less than sequenceLength ^ "tau-s count"
   * Estimation: sequenceLength * l_{ctx} < maxCntBits.
   */
  if (_maxSeqLenLog2 * ctxLength_ > countNumBits) {
    throw std::logic_error("Too big context.");
  }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getWordOrd(const Count& cumulativeNumFound) const -> Ord {
  const auto idxs = std::ranges::iota_view(Ord{0}, this->maxOrd_);
  assert(cumulativeNumFound <= this->getTotalWordsCnt());
  const auto getLowerCumulNumFound_ = [this](Ord ord) {
    assert(ord < maxOrd_);
    return this->getLowerCumulativeCnt_(ord + 1);
  };
  const auto iter = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  assert(ord < maxOrd_);
  auto ret = getProbabilityStats_(ord);
  updateWordCnt_(ord, 1);
  return std::move(ret);
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
      zeroUniqueCnt < this->maxOrd_) {
    total *= this->maxOrd_ - zeroUniqueCnt;
  }
  return total;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  assert(ord <= this->maxOrd_);
  Count lower = 0;
  Count uniqueCountsProd = 1;
  for (auto ctx = getSearchCtxEmptySkipped_(); !ctx.empty(); ctx.pop_back()) {
    const auto& ctxCell = ctxInfo_.at(ctx);
    const auto ctxTotalCnt = ctxCell.cnt.getTotalWordsCnt();
    lower *= ctxTotalCnt * 2;
    const auto lowerCnt = ctxCell.cnt.getLowerCumulativeCount(ord);
    const auto lowerUniqueCnt = ctxCell.uniqueCnt.getLowerCumulativeCount(ord);
    lower += (lowerCnt * 2 - lowerUniqueCnt) * uniqueCountsProd;
    uniqueCountsProd *= ctxCell.uniqueCnt.getTotalWordsCnt();
  }
  const auto zeroCtxTotalUniqueCnt = zeroCtxCell_.uniqueCnt.getTotalWordsCnt();
  if (const auto zeroCtxTotalCnt = zeroCtxCell_.cnt.getTotalWordsCnt();
      zeroCtxTotalCnt != 0) {
    lower *= zeroCtxTotalCnt * 2;
    const auto cumulativeCnt = zeroCtxCell_.cnt.getLowerCumulativeCount(ord);
    const auto cumulativeUniqueCnt =
        zeroCtxCell_.uniqueCnt.getLowerCumulativeCount(ord);
    lower += (cumulativeCnt * 2 - cumulativeUniqueCnt) * uniqueCountsProd;
    uniqueCountsProd *= zeroCtxTotalUniqueCnt;
  }
  if (zeroCtxTotalUniqueCnt < this->maxOrd_) {
    lower *= this->maxOrd_ - zeroCtxTotalUniqueCnt;
    lower += (ord - zeroCtxCell_.uniqueCnt.getLowerCumulativeCount(ord)) *
             uniqueCountsProd;
  }
  return {lower};
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getProbabilityStats_(Ord ord) const -> ProbabilityStats {
  assert(ord < maxOrd_);
  Count lower = 0;
  Count count = 0;
  Count total = 1;
  Count uniqueCountsProd = 1;
  for (auto ctx = getSearchCtxEmptySkipped_(); !ctx.empty(); ctx.pop_back()) {
    const auto& ctxInfo = ctxInfo_.at(ctx).cnt;
    const auto& ctxUniqueInfo = ctxInfo_.at(ctx).uniqueCnt;
    const auto ctxTotalCnt = ctxInfo.getTotalWordsCnt();
    lower *= ctxTotalCnt * 2;
    const auto lowerCnt = ctxInfo.getLowerCumulativeCount(ord);
    const auto lowerUniqueCnt = ctxUniqueInfo.getLowerCumulativeCount(ord);
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
  const auto lowerCnt = zeroCtxCell_.cnt.getLowerCumulativeCount(ord);
  const auto lowerUniqueCnt =
      zeroCtxCell_.uniqueCnt.getLowerCumulativeCount(ord);
  lower += (lowerCnt * 2 - lowerUniqueCnt) * uniqueCountsProd;
  count *= zeroCtxTotal * 2;
  const auto cnt = zeroCtxCell_.cnt.getCount(ord);
  const auto uniqueCnt = zeroCtxCell_.uniqueCnt.getCount(ord);
  count += (cnt * 2 - uniqueCnt) * uniqueCountsProd;
  if (zeroCtxTotal != 0) {
    total *= zeroCtxTotal * 2;
    uniqueCountsProd *= zeroCtxUniqueTotal;
  }
  if (zeroCtxUniqueTotal < this->maxOrd_) {
    total *= this->maxOrd_ - zeroCtxUniqueTotal;
    lower *= this->maxOrd_ - zeroCtxUniqueTotal;
    lower += (ord - zeroCtxCell_.uniqueCnt.getLowerCumulativeCount(ord)) *
             uniqueCountsProd;
    count *= this->maxOrd_ - zeroCtxUniqueTotal;
    count += uniqueCountsProd * (1 - zeroCtxCell_.uniqueCnt.getCount(ord));
  }
  assert(count > 0);
  assert(lower + count <= total);
  return {lower, lower + count, total};
}

////////////////////////////////////////////////////////////////////////////////
void PPMDDictionary::updateWordCnt_(
    Ord ord, ael::impl::dict::CumulativeCount::Count cnt) {
  for (auto ctx = SearchCtx_(ctx_.rbegin(), ctx_.rend()); !ctx.empty();
       ctx.pop_back()) {
    if (!ctxInfo_.contains(ctx)) {
      ctxInfo_.emplace(
          ctx, CtxCell_{ael::impl::dict::CumulativeCount(maxOrd_),
                        ael::impl::dict::CumulativeUniqueCount(maxOrd_)});
    }
    ctxInfo_.at(ctx).cnt.increaseOrdCount(ord, static_cast<std::int64_t>(cnt));
    ctxInfo_.at(ctx).uniqueCnt.update(ord);
  }
  zeroCtxCell_.cnt.increaseOrdCount(ord, static_cast<std::int64_t>(cnt));
  zeroCtxCell_.uniqueCnt.update(ord);
  ctx_.push_back(ord);
  if (ctx_.size() > ctxLength_) {
    ctx_.pop_front();
  }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getSearchCtxEmptySkipped_() const -> SearchCtx_ {
  auto ctx = SearchCtx_(ctx_.rbegin(), ctx_.rend());
  for (; !ctx.empty() && !ctxInfo_.contains(ctx); ctx.pop_back()) {
    // Skip contexts which were not found yet.
  }
  return ctx;
}

}  // namespace ael::dict
