#include <ael/dictionary/adaptive_dictionary.hpp>

#include "integer_random_access_iterator.hpp"
#include <boost/range/iterator_range.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveDictionary::AdaptiveDictionary(Ord maxOrd, std::uint64_t ratio)
    : impl::AdaptiveDictionaryBase<Count>(maxOrd, maxOrd),
      _ratio(ratio),
      _maxOrder(maxOrd) {}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    using UintIt = ael::impl::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, _maxOrder);

    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return _getLowerCumulativeCnt(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return it - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + this->_wordCnts[ord] * _ratio + 1;
    const auto total = getTotalWordsCnt();
    _updateWordCnt(ord);
    return { low, high, total };
}

////////////////////////////////////////////////////////////////////////////////
void AdaptiveDictionary::_updateWordCnt(Ord ord) {
    this->_cumulativeWordCounts.update(ord, _maxOrder, 1);
    ++this->_wordCnts[ord];
    this->_totalWordsCnt += _ratio;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::_getLowerCumulativeCnt(Ord ord) const -> Count {
    return ord + this->_cumulativeWordCounts.get(ord - 1) * _ratio;
}

}  // namespace ael::dict
