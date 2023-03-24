#include <ael/dictionary/ppmd_dictionary.hpp>

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <boost/range/iterator_range.hpp>

#include "ael/dictionary/impl/cumulative_count.hpp"
#include "ael/dictionary/impl/cumulative_unique_count.hpp"
#include "integer_random_access_iterator.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
PPMDDictionary::PPMDDictionary(Ord maxOrd, std::size_t ctxLength)
    : _maxOrd(maxOrd),
      _zeroCtxCell{ maxOrd, maxOrd },
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
auto PPMDDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
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
auto PPMDDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    assert(ord < _maxOrd);
    const auto ret = _getProbabilityStats(ord);
    _updateWordCnt(ord, 1);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getTotalWordsCnt() const -> Count {
    Count total = 1;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto totalCnt = _ctxInfo.at(ctx).cnt.getTotalWordsCnt();
        total *= totalCnt * 2;
    }
    if (const auto totalZeroCtxCnt = _zeroCtxCell.cnt.getTotalWordsCnt();
            totalZeroCtxCnt != 0) {
        total *= totalZeroCtxCnt * 2;
    }
    if (const auto zeroUniqueCnt = _zeroCtxCell.uniqueCnt.getTotalWordsCnt();
            zeroUniqueCnt < this->_maxOrd) {
        total *= this->_maxOrd - zeroUniqueCnt;
    }
    return total;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::_getLowerCumulativeCnt(Ord ord) const -> Count {
    assert(ord <= this->_maxOrd);
    Count lower = 0;
    Count uniqueCountsProd = 1;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto& ctxCell = _ctxInfo.at(ctx);
        const auto ctxTotalCnt = ctxCell.cnt.getTotalWordsCnt();
        lower *= ctxTotalCnt * 2;
        const auto lowerCnt = ctxCell.cnt.getLowerCumulativeCount(ord);
        const auto lowerUniqueCnt =
            ctxCell.uniqueCnt.getLowerCumulativeCount(ord);
        lower += (lowerCnt * 2 - lowerUniqueCnt) * uniqueCountsProd;
        uniqueCountsProd *= ctxCell.uniqueCnt.getTotalWordsCnt();
    }
    const auto zeroCtxTotalUniqueCnt =
        _zeroCtxCell.uniqueCnt.getTotalWordsCnt();
    if (const auto zeroCtxTotalCnt = _zeroCtxCell.cnt.getTotalWordsCnt();
            zeroCtxTotalCnt != 0) {
        lower *= zeroCtxTotalCnt * 2;
        const auto cumulativeCnt =
            _zeroCtxCell.cnt.getLowerCumulativeCount(ord);
        const auto cumulativeUniqueCnt =
            _zeroCtxCell.uniqueCnt.getLowerCumulativeCount(ord);
        lower += (cumulativeCnt * 2 - cumulativeUniqueCnt) * uniqueCountsProd;
        uniqueCountsProd *= zeroCtxTotalUniqueCnt;
    }
    if (zeroCtxTotalUniqueCnt < this->_maxOrd) {
        lower *= this->_maxOrd - zeroCtxTotalUniqueCnt;
        lower += (ord - _zeroCtxCell.uniqueCnt.getLowerCumulativeCount(ord)) 
            * uniqueCountsProd;
    }
    return {lower};
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::_getProbabilityStats(Ord ord) const -> ProbabilityStats {
    assert(ord < _maxOrd);
    Count lower = 0;
    Count count = 0;
    Count total = 1;
    Count uniqueCountsProd = 1;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto& ctxInfo = _ctxInfo.at(ctx).cnt;
        const auto& ctxUniqueInfo = _ctxInfo.at(ctx).uniqueCnt;
        const auto ctxTotalCnt = ctxInfo.getTotalWordsCnt();
        lower *= ctxTotalCnt * 2;
        const auto lowerCnt = ctxInfo.getLowerCumulativeCount(ord);
        const auto lowerUniqueCnt =
            ctxUniqueInfo.getLowerCumulativeCount(ord);
        lower += (lowerCnt * 2 - lowerUniqueCnt) * uniqueCountsProd;
        total *= ctxTotalCnt * 2;
        count *= ctxTotalCnt * 2;
        const auto cnt = ctxInfo.getCount(ord);
        const auto uniqueCnt = ctxUniqueInfo.getCount(ord);
        count += (cnt * 2 - uniqueCnt) * uniqueCountsProd;
        uniqueCountsProd *= ctxUniqueInfo.getTotalWordsCnt();
    }
    const auto zeroCtxUniqueTotal = _zeroCtxCell.uniqueCnt.getTotalWordsCnt();
    const auto zeroCtxTotal = _zeroCtxCell.cnt.getTotalWordsCnt();
    lower *= zeroCtxTotal * 2;
    const auto lowerCnt = _zeroCtxCell.cnt.getLowerCumulativeCount(ord);
    const auto lowerUniqueCnt =
        _zeroCtxCell.uniqueCnt.getLowerCumulativeCount(ord);
    lower += (lowerCnt * 2 - lowerUniqueCnt) * uniqueCountsProd;
    count *= zeroCtxTotal * 2;
    const auto cnt = _zeroCtxCell.cnt.getCount(ord);
    const auto uniqueCnt = _zeroCtxCell.uniqueCnt.getCount(ord);
    count += (cnt * 2 - uniqueCnt) * uniqueCountsProd;
    if (zeroCtxTotal != 0) {
        total *= zeroCtxTotal * 2;
        uniqueCountsProd *= zeroCtxUniqueTotal;
    }
    if (zeroCtxUniqueTotal < this->_maxOrd) {
        total *= this->_maxOrd - zeroCtxUniqueTotal;
        lower *= this->_maxOrd - zeroCtxUniqueTotal;
        lower += (ord - _zeroCtxCell.uniqueCnt.getLowerCumulativeCount(ord))
            * uniqueCountsProd;
        count *= this->_maxOrd - zeroCtxUniqueTotal;
        count += uniqueCountsProd * (1 - _zeroCtxCell.uniqueCnt.getCount(ord));
    }
    assert(count > 0);
    assert(lower + count <= total);
    return {lower, lower + count, total};
}

////////////////////////////////////////////////////////////////////////////////
void PPMDDictionary::_updateWordCnt(Ord ord,
                                    impl::CumulativeCount::Count cnt) {
    for (auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
         !ctx.empty();
         ctx.pop_back()) {
        if (!_ctxInfo.contains(ctx)) {
            _ctxInfo.emplace(ctx, _CtxCell{_maxOrd, _maxOrd});
        }
        _ctxInfo.at(ctx).cnt.increaseOrdCount(ord, cnt);
        _ctxInfo.at(ctx).uniqueCnt.update(ord);
    }
    _zeroCtxCell.cnt.increaseOrdCount(ord, cnt);
    _zeroCtxCell.uniqueCnt.update(ord);
    _ctx.push_back(ord);
    if (_ctx.size() > _ctxLength) {
        _ctx.pop_front();
    }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::_getSearchCtxEmptySkipped() const -> _SearchCtx {
    auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
    for (;
         !ctx.empty() && !_ctxInfo.contains(ctx);
         ctx.pop_back()) {
        // Skip contexts which were not found yet.
    }
    return ctx;
}

}  // namespace ael::dict
