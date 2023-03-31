#include <ael/dictionary/adaptive_d_dictionary.hpp>
#include <boost/range/iterator_range.hpp>

#include "integer_random_access_iterator.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveDDictionary::AdaptiveDDictionary(Ord maxOrd)
    : impl::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
  using UIntIt = ael::impl::IntegerRandomAccessIterator<std::uint64_t>;
  const auto idxs =
      boost::iterator_range<UIntIt>(UIntIt{0}, UIntIt{this->getMaxOrd_()});
  // TODO(gogagum): replace
  // auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
  const auto getLowerCumulNumFound_ = [this](Ord ord) {
    return this->getLowerCumulativeCnt_(ord + 1);
  };
  const auto iter = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                           getLowerCumulNumFound_);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  const auto ret = getProbabilityStats_(ord);
  this->_updateWordCnt(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getTotalWordsCnt() const -> Count {
  const auto totalWordsCnt = this->_getRealTotalWordsCnt();
  if (totalWordsCnt == 0) {
    return this->getMaxOrd_();
  }
  const auto totalWordsUniqueCnt = this->_getTotalWordsUniqueCnt();
  if (totalWordsUniqueCnt == this->getMaxOrd_()) {
    return totalWordsCnt;
  }
  return 2 * (this->getMaxOrd_() - totalWordsUniqueCnt) * totalWordsCnt;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  if (this->_getRealTotalWordsCnt() == 0) {
    return ord;
  }
  const auto totalUniqueWordsCnt = this->_getTotalWordsUniqueCnt();
  const auto cumulativeWordsCnt = this->_getRealLowerCumulativeWordCnt(ord);
  if (totalUniqueWordsCnt == this->getMaxOrd_()) {
    return cumulativeWordsCnt;
  }
  return (this->getMaxOrd_() - totalUniqueWordsCnt) * 2 * cumulativeWordsCnt +
         ord * totalUniqueWordsCnt -
         this->getMaxOrd_() * this->_getLowerCumulativeUniqueNumFound(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordCnt_(Ord ord) const -> Count {
  if (this->_getRealTotalWordsCnt() == 0) {
    return 1;
  }
  const auto totalUniqueWordsCount = this->_getTotalWordsUniqueCnt();
  const auto realWordCnt = this->_getRealWordCnt(ord);
  if (totalUniqueWordsCount == this->getMaxOrd_()) {
    return realWordCnt;
  }
  return (this->getMaxOrd_() - totalUniqueWordsCount) * 2 * realWordCnt +
         totalUniqueWordsCount - this->getMaxOrd_() * _getWordUniqueCnt(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getProbabilityStats_(Ord ord) const
    -> ProbabilityStats {
  const auto low = getLowerCumulativeCnt_(ord);
  const auto high = low + getWordCnt_(ord);
  const auto total = getTotalWordsCnt();
  return {low, high, total};
}

}  // namespace ael::dict
