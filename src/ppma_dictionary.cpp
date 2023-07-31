#include <ael/dictionary/ppma_dictionary.hpp>
#include <ael/impl/dictionary/cumulative_count.hpp>
#include <algorithm>
#include <ranges>
#include <stdexcept>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
PPMADictionary::PPMADictionary(ConstructInfo constructInfo)
    : maxOrd_(constructInfo.maxOrd),
      zeroCtxCnt_(constructInfo.maxOrd),
      zeroCtxUniqueCnt_(constructInfo.maxOrd),
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
auto PPMADictionary::getWordOrd(const Count& cumulativeNumFound) const -> Ord {
  const auto idxs = std::ranges::iota_view(Ord{0}, maxOrd_);
  assert(cumulativeNumFound <= getTotalWordsCnt());
  const auto getLowerCumulCnt = [this](std::uint64_t ord) {
    assert(ord < maxOrd_);
    return getLowerCumulativeCnt_(ord + 1);
  };
  const auto iter =
      std::ranges::upper_bound(idxs, cumulativeNumFound, {}, getLowerCumulCnt);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  assert(ord < maxOrd_);
  auto ret = getProbabilityStats_(ord);
  updateWordCnt_(ord, 1);
  return std::move(ret);
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getTotalWordsCnt() const -> Count {
  Count total = 1;
  for (auto ctx = getSearchCtxEmptySkipped_(); !ctx.empty(); ctx.pop_back()) {
    const auto totalCnt = ctxInfo_.at(ctx).getTotalWordsCnt();
    total *= totalCnt + 1;
  }
  total *= zeroCtxCnt_.getTotalWordsCnt() + 1;
  if (const auto zeroUniqueCnt = zeroCtxUniqueCnt_.getTotalWordsCnt();
      zeroUniqueCnt < maxOrd_) {
    total *= maxOrd_ - zeroUniqueCnt;
  }
  return total;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  Count lower = 0;
  for (auto ctx = getSearchCtxEmptySkipped_(); !ctx.empty(); ctx.pop_back()) {
    const auto& ctxInfo = ctxInfo_.at(ctx);
    const auto ctxTotalCnt = ctxInfo.getTotalWordsCnt();
    lower *= ctxTotalCnt + 1;
    lower += ctxInfo.getLowerCumulativeCount(ord);
  }
  lower *= zeroCtxCnt_.getTotalWordsCnt() + 1;
  lower += zeroCtxCnt_.getLowerCumulativeCount(ord);
  if (const auto zeroUniqueCnt = zeroCtxUniqueCnt_.getTotalWordsCnt();
      zeroUniqueCnt < maxOrd_) {
    lower *= maxOrd_ - zeroUniqueCnt;
    lower += ord - zeroCtxUniqueCnt_.getLowerCumulativeCount(ord);
  }
  return {lower};
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getProbabilityStats_(Ord ord) const -> ProbabilityStats {
  Count lower = 0;
  Count count = 0;
  Count total = 1;
  for (auto ctx = getSearchCtxEmptySkipped_(); !ctx.empty(); ctx.pop_back()) {
    const auto& ctxInfo = ctxInfo_.at(ctx);
    const auto ctxTotalCnt = ctxInfo.getTotalWordsCnt();
    lower *= ctxTotalCnt + 1;
    total *= ctxTotalCnt + 1;
    lower += ctxInfo.getLowerCumulativeCount(ord);
    count *= ctxTotalCnt + 1;
    count += ctxInfo.getCount(ord);
  }
  lower *= zeroCtxCnt_.getTotalWordsCnt() + 1;
  total *= zeroCtxCnt_.getTotalWordsCnt() + 1;
  lower += zeroCtxCnt_.getLowerCumulativeCount(ord);
  count *= zeroCtxCnt_.getTotalWordsCnt() + 1;
  count += zeroCtxCnt_.getCount(ord);
  if (const auto zeroUniqueCnt = zeroCtxUniqueCnt_.getTotalWordsCnt();
      zeroUniqueCnt < maxOrd_) {
    total *= maxOrd_ - zeroUniqueCnt;
    lower *= maxOrd_ - zeroUniqueCnt;
    lower += ord - zeroCtxUniqueCnt_.getLowerCumulativeCount(ord);
    count *= maxOrd_ - zeroUniqueCnt;
    count += 1 - zeroCtxUniqueCnt_.getCount(ord);
  }
  assert(count > 0);
  assert(lower + count <= total);
  return {lower, lower + count, total};
}

////////////////////////////////////////////////////////////////////////////////
void PPMADictionary::updateWordCnt_(
    Ord ord, ael::impl::dict::CumulativeCount::Count cnt) {
  for (auto ctx = SearchCtx_(ctx_.rbegin(), ctx_.rend()); !ctx.empty();
       ctx.pop_back()) {
    if (!ctxInfo_.contains(ctx)) {
      ctxInfo_.emplace(ctx, maxOrd_);
    }
    ctxInfo_.at(ctx).increaseOrdCount(ord, static_cast<std::int64_t>(cnt));
  }
  zeroCtxCnt_.increaseOrdCount(ord, static_cast<std::int64_t>(cnt));
  zeroCtxUniqueCnt_.update(ord);
  ctx_.push_back(ord);
  if (ctx_.size() > ctxLength_) {
    ctx_.pop_front();
  }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getSearchCtxEmptySkipped_() const -> SearchCtx_ {
  auto ctx = SearchCtx_(ctx_.rbegin(), ctx_.rend());
  for (; !ctx.empty() && !ctxInfo_.contains(ctx); ctx.pop_back()) {
    // Skip contexts which were not found yet.
  }
  return ctx;
}

}  // namespace ael::dict
