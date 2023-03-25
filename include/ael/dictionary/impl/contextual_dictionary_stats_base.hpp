#ifndef CONTEXTUAL_DICTIONARY_STATS_BASE_HPP
#define CONTEXTUAL_DICTIONARY_STATS_BASE_HPP

#include "word_probability_stats.hpp"
#include <cstdint>
#include <boost/multiprecision/cpp_int.hpp>
#include <unordered_map>

namespace ael::dict::impl {

namespace bm = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ContextualDictionaryStatsBase<InternalDict> class
template <class InternalDict>
class ContextualDictionaryStatsBase : protected InternalDict {
protected:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using Dict_ = InternalDict;

    struct SearchCtx_ {
        std::uint16_t length;
        Ord ctx;
        friend bool operator==(SearchCtx_, SearchCtx_) = default;
    };

    struct SearchCtxHash_ {
        std::size_t operator()(SearchCtx_ searchCtx) const {
            return static_cast<std::size_t>(searchCtx.ctx)
                ^ std::size_t{searchCtx.length};
        }
    };

public:

    /**
     * @brief contextual dictionary constructor.
     * @param wordNumBits word bits length.
     * @param ctxLength number of blocks in context.
     * @param ctxCellBitsLength context cell bits length.
     */
    ContextualDictionaryStatsBase(std::uint16_t wordNumBits,
                                  std::uint16_t ctxLength,
                                  std::uint16_t ctxCellBitsLength);
protected:

    Ord _getCtx(std::uint16_t length) const;

    SearchCtx_ _getSearchCtx(std::uint16_t length) const;

    void _updateCtx(Ord ord);

    Count _getContextualTotalWordCnt(const SearchCtx_& searchCtx) const;

    Ord _getContextualWordOrd(const SearchCtx_& searchCtx,
                              Count cumulativeCnt) const;

    void _updateContextualDictionary(const SearchCtx_& searchCtx, Ord ord);

    WordProbabilityStats<Count>
    _getContextualProbStats(const SearchCtx_& searchCtx, Ord ord);

protected:
    std::unordered_map<SearchCtx_, Dict_, SearchCtxHash_> _contextProbs;
    const std::uint16_t _ctxCellBitsLength;
    const std::uint16_t _ctxLength;
    const std::uint16_t _numBits;
    Ord _ctx;
    std::uint16_t _currCtxLength;
};

////////////////////////////////////////////////////////////////////////////////
template<class InternalDictT>
ContextualDictionaryStatsBase<InternalDictT>::ContextualDictionaryStatsBase(
    std::uint16_t wordBitsLength,
    std::uint16_t ctxLength,
    std::uint16_t ctxCellBitsLength) 
        : _ctxCellBitsLength(ctxCellBitsLength),
          _ctxLength(ctxLength),
          _numBits(wordBitsLength),
          _ctx(0),
          _currCtxLength(0),
          InternalDictT(1ull << wordBitsLength) {
    if (ctxCellBitsLength * ctxLength > 56) {
        throw std::invalid_argument("Too big context length.");
    }
}

////////////////////////////////////////////////////////////////////////////////
template<class InternalDictT>
auto ContextualDictionaryStatsBase<InternalDictT>::_getCtx(
        std::uint16_t len) const -> Ord {
    return _ctx % (1ull << (_ctxCellBitsLength * len));
}

////////////////////////////////////////////////////////////////////////////////
template<class InternalDictT>
auto ContextualDictionaryStatsBase<InternalDictT>::_getSearchCtx(
        std::uint16_t len) const -> SearchCtx_ {
    return {len, _getCtx(len)};
}

////////////////////////////////////////////////////////////////////////////////
template<class InternalDictT>
void ContextualDictionaryStatsBase<InternalDictT>::_updateCtx(Ord ord) {
    if (_currCtxLength < _ctxLength) {
        ++_currCtxLength;
    }
    bm::uint128_t newCtx128 = (_ctx != 0) ? _ctx - 1 : 0;
    newCtx128 *= (1ull << _numBits);
    newCtx128 += ord;
    newCtx128 %= (1ull << (_ctxCellBitsLength * _ctxLength));
    _ctx = newCtx128.convert_to<Ord>();
}

////////////////////////////////////////////////////////////////////////////////
template<class InternalDictT>
auto
ContextualDictionaryStatsBase<InternalDictT>::_getContextualTotalWordCnt(
        const SearchCtx_& searchCtx) const -> Count {
    if (!this->_contextProbs.contains(searchCtx)) {
        return 0;
    }
    return this->_contextProbs.at(searchCtx)._getRealTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
auto ContextualDictionaryStatsBase<InternalDictT>::_getContextualWordOrd(
        const SearchCtx_& searchCtx, Count cumulativeCnt) const -> Ord {
    return this->_contextProbs.at(searchCtx).getWordOrd(cumulativeCnt);
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
void
ContextualDictionaryStatsBase<InternalDictT>::_updateContextualDictionary(
        const SearchCtx_& searchCtx, Ord ord) {
    if (!this->_contextProbs.contains(searchCtx)) {
        this->_contextProbs.emplace(searchCtx, this->_maxOrd);
    }
    this->_contextProbs.at(searchCtx)._updateWordCnt(ord, 1);
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
auto
ContextualDictionaryStatsBase<InternalDictT>::_getContextualProbStats(
        const SearchCtx_& searchCtx,
        Ord ord) -> WordProbabilityStats<Count>{
    return this->_contextProbs.at(searchCtx).getProbabilityStats(ord);
}

}  // ael::dict::impl

#endif  // CONTEXTUAL_DICTIONARY_STATS_BASE_HPP
