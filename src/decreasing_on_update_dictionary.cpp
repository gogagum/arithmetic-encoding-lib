#include <ael/dictionary/decreasing_on_update_dictionary.hpp>
#include <ranges>

#include "integer_random_access_iterator.hpp"
#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
DecreasingOnUpdateDictionary::DecreasingOnUpdateDictionary(
        Ord maxOrd,
        Count count
        ) : impl::AdaptiveDictionaryBase<Count>(maxOrd, maxOrd * count),
            _maxOrd(maxOrd) {
    for (auto ord : boost::irange<Ord>(0, _maxOrd)) {
        this->_wordCnts[ord] = count;
        this->_cumulativeWordCounts.update(ord, _maxOrd, count);
    }
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::getWordOrd(
        Count cumulativeNumFound) const -> Ord {
    using UintIt = ael::impl::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, _maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    const auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
    return it - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::getProbabilityStats(
        Ord ord) -> ProbabilityStats {
    auto ret = _getProbabilityStats(ord);
    _updateWordCnt(ord, 1);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::_getLowerCumulativeCnt(
        Ord ord) const -> Count {
    if (ord == 0) {
        return Count{0};
    }
    return this->_cumulativeWordCounts.get(ord - 1);
}

////////////////////////////////////////////////////////////////////////////////
void DecreasingOnUpdateDictionary::_updateWordCnt(Ord ord, Count cnt) {
    this->_totalWordsCnt -= 1;
    this->_cumulativeWordCounts.update(
        ord, _maxOrd, -static_cast<std::int64_t>(cnt));
    --this->_wordCnts[ord];
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::_getProbabilityStats(
        Ord ord) const -> ProbabilityStats {
    assert(this->_wordCnts.contains(ord));
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + this->_wordCnts.at(ord);
    const auto total = getTotalWordsCnt();
    return {low, high, total};
}

}  // namespace ael::dict
