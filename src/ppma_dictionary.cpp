#include <ael/dictionary/ppma_dictionary.hpp>

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <boost/range/iterator_range.hpp>

#include "ael/dictionary/impl/cumulative_count.hpp"
#include "integer_random_access_iterator.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
PPMADictionary::PPMADictionary(Ord maxOrd, std::size_t ctxLength)
    : _maxOrd(maxOrd),
      _zeroCtxCnt(maxOrd),
      _zeroCtxUniqueCnt(maxOrd),
      _ctxLength(ctxLength) {
    /**
     * \tau_{ctx}_{i} < sequenceLength
     * Product of tau-s must be less than sequenceLength ^ "tau-s count"
     * Estimation: sequenceLength * l_{ctx} < maxCntBits.
     */
    if (_maxSeqLenLog2 * ctxLength > countNumBits) {
        throw std::logic_error("Too big context.");
    }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    using UintIt = ael::impl::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, this->_maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    assert(cumulativeNumFound <= this->getTotalWordsCnt());
    const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
        assert(ord < _maxOrd);
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    const auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
    auto t = (it - idxs.begin());
    assert(t < _maxOrd);
    return it - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    assert(ord < _maxOrd);
    const auto ret = _getProbabilityStats(ord);
    _updateWordCnt(ord, 1);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getTotalWordsCnt() const -> Count {
    Count total = 1;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto totalCnt = _ctxInfo.at(ctx).getTotalWordsCnt();  
        total *= totalCnt + 1;
    }
    total *= _zeroCtxCnt.getTotalWordsCnt() + 1;
    if (const auto zeroUniqueCnt = _zeroCtxUniqueCnt.getTotalWordsCnt();
            zeroUniqueCnt < this->_maxOrd) {
        total *= this->_maxOrd - zeroUniqueCnt;
    }
    return total;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::_getLowerCumulativeCnt(Ord ord) const -> Count {
    Count lower = 0;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto& ctxInfo = _ctxInfo.at(ctx);
        const auto ctxTotalCnt = ctxInfo.getTotalWordsCnt();
        lower *= ctxTotalCnt + 1;
        lower += ctxInfo.getLowerCumulativeCount(ord);
    }
    lower *= _zeroCtxCnt.getTotalWordsCnt() + 1;
    lower += _zeroCtxCnt.getLowerCumulativeCount(ord);
    if (const auto zeroUniqueCnt = _zeroCtxUniqueCnt.getTotalWordsCnt();
            zeroUniqueCnt < this->_maxOrd) {
        lower *= this->_maxOrd - zeroUniqueCnt;
        lower += ord - _zeroCtxUniqueCnt.getLowerCumulativeCount(ord);
    }
    return {lower};
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::_getProbabilityStats(Ord ord) const -> ProbabilityStats {
    Count lower = 0;
    Count count = 0;
    Count total = 1;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto& ctxInfo = _ctxInfo.at(ctx);
        const auto ctxTotalCnt = ctxInfo.getTotalWordsCnt();
        lower *= ctxTotalCnt + 1;
        total *= ctxTotalCnt + 1;
        lower += ctxInfo.getLowerCumulativeCount(ord);
        count *= ctxTotalCnt + 1;
        count += ctxInfo.getCount(ord);
    }
    lower *= _zeroCtxCnt.getTotalWordsCnt() + 1;
    total *= _zeroCtxCnt.getTotalWordsCnt() + 1;
    lower += _zeroCtxCnt.getLowerCumulativeCount(ord);
    count *= _zeroCtxCnt.getTotalWordsCnt() + 1;
    count += _zeroCtxCnt.getCount(ord);
    if (const auto zeroUniqueCnt = _zeroCtxUniqueCnt.getTotalWordsCnt();
            zeroUniqueCnt < this->_maxOrd) {
        total *= this->_maxOrd - zeroUniqueCnt;
        lower *= this->_maxOrd - zeroUniqueCnt;
        lower += ord - _zeroCtxUniqueCnt.getLowerCumulativeCount(ord);
        count *= this->_maxOrd - zeroUniqueCnt;
        count += 1 - _zeroCtxUniqueCnt.getCount(ord);
    }
    assert(count > 0);
    assert(lower + count <= total);
    return {lower, lower + count, total};
}

////////////////////////////////////////////////////////////////////////////////
void PPMADictionary::_updateWordCnt(Ord ord,
                                    impl::CumulativeCount::Count cnt) {
    for (auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
         !ctx.empty();
         ctx.pop_back()) {
        if (!_ctxInfo.contains(ctx)) {
            _ctxInfo.emplace(ctx, _maxOrd);
        }
        _ctxInfo.at(ctx).increaseOrdCount(ord, cnt);
    }
    _zeroCtxCnt.increaseOrdCount(ord, cnt);
    _zeroCtxUniqueCnt.update(ord);
    _ctx.push_back(ord);
    if (_ctx.size() > _ctxLength) {
        _ctx.pop_front();
    }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::_getSearchCtxEmptySkipped() const -> _SearchCtx {
    auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
    for (;
         !ctx.empty() && !_ctxInfo.contains(ctx);
         ctx.pop_back()) {
        // Skip contexts which were not found yet.
    }
    return ctx;
}

}  // namespace ael::dict
