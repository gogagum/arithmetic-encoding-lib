#include <ael/dictionary/adaptive_dictionary.hpp>
#include <boost/range/iterator_range.hpp>

#include "integer_random_access_iterator.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveDictionary::AdaptiveDictionary(ConstructInfo constructInfo)
    : impl::AdaptiveDictionaryBase<Count>(constructInfo.maxOrd,
                                          constructInfo.maxOrd),
      _ratio(constructInfo.ratio),
      _maxOrder(constructInfo.maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
  using UIntIt = ael::impl::IntegerRandomAccessIterator<std::uint64_t>;
  const auto idxs = boost::iterator_range<UIntIt>(UIntIt{0}, UIntIt{_maxOrder});
  // TODO(gogagum): replace
  // auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
  const auto getLowerCumulNumFound_ = [this](Ord ord) {
    return getLowerCumulativeCnt_(ord + 1);
  };
  const auto iter = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                     getLowerCumulNumFound_);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  const auto low = getLowerCumulativeCnt_(ord);
  const auto high = low + this->_wordCnts[ord] * _ratio + 1;
  const auto total = getTotalWordsCnt();
  updateWordCnt_(ord);
  return {low, high, total};
}

////////////////////////////////////////////////////////////////////////////////
void AdaptiveDictionary::updateWordCnt_(Ord ord) {
  this->_cumulativeWordCounts.update(ord, _maxOrder, 1);
  ++this->_wordCnts[ord];
  this->_totalWordsCnt += _ratio;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  return ord + this->_cumulativeWordCounts.get(ord - 1) * _ratio;
}

}  // namespace ael::dict
