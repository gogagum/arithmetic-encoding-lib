#include <ael/dictionary/adaptive_d_dictionary.hpp>

#include "integer_random_access_iterator.hpp"
#include <boost/range/iterator_range.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveDDictionary::AdaptiveDDictionary(Ord maxOrd)
    : impl::ADDictionaryBase(maxOrd) {}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    using UIntIt = ael::impl::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::iterator_range<UIntIt>(
        UIntIt{0}, UIntIt{this->_maxOrd});
    // TODO(gogagum): replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return this->getLowerCumulativeCnt_(ord + 1);
    };
    const auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
    return it - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    const auto ret = getProbabilityStats_(ord);
    this->_updateWordCnt(ord, 1);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getTotalWordsCnt() const -> Count {
    const auto totalWordsCnt = this->_cumulativeCnt.getTotalWordsCnt(); 
    if (totalWordsCnt == 0) {
        return this->_maxOrd;
    }
    const auto totalWordsUniqueCnt = this->_getTotalWordsUniqueCnt();  
    if (totalWordsUniqueCnt == this->_maxOrd) {
        return totalWordsCnt;
    }
    return 2 * (this->_maxOrd - totalWordsUniqueCnt) * totalWordsCnt;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
    if (this->_cumulativeCnt.getTotalWordsCnt() == 0) {
        return ord;
    }
    const auto totalUniqueWordsCnt = this->_getTotalWordsUniqueCnt();
    const auto cumulativeWordsCnt = this->_getRealLowerCumulativeWordCnt(ord);
    if (totalUniqueWordsCnt == this->_maxOrd) {
        return cumulativeWordsCnt;
    }
    return (this->_maxOrd - totalUniqueWordsCnt) * 2 * cumulativeWordsCnt
            + ord * totalUniqueWordsCnt
            - this->_maxOrd * this->_getLowerCumulativeUniqueNumFound(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordCnt_(Ord ord) const -> Count {
    if (this->_cumulativeCnt.getTotalWordsCnt() == 0) {
        return 1;
    }
    const auto totalUniqueWordsCount = this->_getTotalWordsUniqueCnt();
    const auto realWordCnt = this->_getRealWordCnt(ord); 
    if (totalUniqueWordsCount == this->_maxOrd) {
        return realWordCnt;
    }
    return (this->_maxOrd - totalUniqueWordsCount) * 2 * realWordCnt
        + totalUniqueWordsCount
        - this->_maxOrd * _getWordUniqueCnt(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats_(
        Ord ord) const -> ProbabilityStats {
    const auto low = getLowerCumulativeCnt_(ord);
    const auto high = low + getWordCnt_(ord);
    const auto total = getTotalWordsCnt();
    return { low, high, total };
}

}  // namespace ael::dict
