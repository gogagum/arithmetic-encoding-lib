#include <ael/dictionary/adaptive_a_dictionary.hpp>

#include "integer_random_access_iterator.hpp"
#include <boost/range/iterator_range.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveADictionary::AdaptiveADictionary(Ord maxOrd)
    : impl::ADDictionaryBase(maxOrd) {}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    using UintIt = ael::impl::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, this->_maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    const auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
    return it - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    const auto ret = _getProbabilityStats(ord);
    this->_updateWordCnt(ord, 1);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getTotalWordsCnt() const -> Count {
    const auto uniqueWordsCnt = this->_getTotalWordsUniqueCnt();
    const auto wordsCnt = this->_getRealTotalWordsCnt();
    if (this->_maxOrd == uniqueWordsCnt) {
        return wordsCnt;
    }
    return (this->_maxOrd - uniqueWordsCnt) * (wordsCnt + 1);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::_getLowerCumulativeCnt(
        Ord ord) const -> Count {
    const auto cumulativeNumFound = this->_getRealLowerCumulativeWordCnt(ord);
    if (this->_maxOrd == this->_getTotalWordsUniqueCnt()) {
        return cumulativeNumFound;
    }
    const auto numUniqueWordsTotal = this->_getTotalWordsUniqueCnt();
    const auto cumulativeUniqueWordsNumFound =
        this->_getLowerCumulativeUniqueNumFound(ord);
    return (this->_maxOrd - numUniqueWordsTotal) * cumulativeNumFound
            + (ord - cumulativeUniqueWordsNumFound);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::_getWordCnt(Ord ord) const -> Count {
    const auto totalUniqueWordsCnt = this->_getTotalWordsUniqueCnt();
    if (this->_maxOrd == totalUniqueWordsCnt) {
        return this->_cumulativeCnt.getCount(ord);
    }
    return this->_cumulativeUniqueCnt.getCount(ord) == 1
           ? this->_cumulativeCnt.getCount(ord)
             * (this->_maxOrd - totalUniqueWordsCnt)
           : 1;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::_getProbabilityStats(
        Ord ord) const -> ProbabilityStats {
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + _getWordCnt(ord);
    const auto total = getTotalWordsCnt();
    assert(high > low);
    assert(total >= high);
    return { low, high, total };
}

}  // namespace ael::dict
