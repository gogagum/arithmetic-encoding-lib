#include <ael/dictionary/decreasing_on_update_dictionary.hpp>
#include <algorithm>
#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>
#include <ranges>

#include "integer_random_access_iterator.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
DecreasingOnUpdateDictionary::DecreasingOnUpdateDictionary(Ord maxOrd,
                                                           Count count)
    : impl::AdaptiveDictionaryBase<Count>(maxOrd, maxOrd * count),
      maxOrd_(maxOrd) {
  for (auto ord : boost::irange<Ord>(0, maxOrd_)) {
    this->_wordCnts[ord] = count;
    this->_cumulativeWordCounts.update(ord, maxOrd_, count);
  }
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::getWordOrd(Count cumulativeNumFound) const
    -> Ord {
  using UIntIt = ael::impl::IntegerRandomAccessIterator<std::uint64_t>;
  const auto idxs = boost::iterator_range<UIntIt>(UIntIt{0}, UIntIt{maxOrd_});
  // TODO(gogagum): replace
  // auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
  const auto getLowerCumulNumFound_ = [this](Ord ord) {
    return this->_getLowerCumulativeCnt(ord + 1);
  };
  const auto iter = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::getProbabilityStats(Ord ord)
    -> ProbabilityStats {
  auto ret = _getProbabilityStats(ord);
  _updateWordCnt(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::_getLowerCumulativeCnt(Ord ord) const
    -> Count {
  if (ord == 0) {
    return Count{0};
  }
  return this->_cumulativeWordCounts.get(ord - 1);
}

////////////////////////////////////////////////////////////////////////////////
void DecreasingOnUpdateDictionary::_updateWordCnt(Ord ord, Count cnt) {
  this->_totalWordsCnt -= 1;
  this->_cumulativeWordCounts.update(ord, maxOrd_,
                                     -static_cast<std::int64_t>(cnt));
  --this->_wordCnts[ord];
}

////////////////////////////////////////////////////////////////////////////////
auto DecreasingOnUpdateDictionary::_getProbabilityStats(Ord ord) const
    -> ProbabilityStats {
  assert(this->_wordCnts.contains(ord));
  const auto low = _getLowerCumulativeCnt(ord);
  const auto high = low + this->_wordCnts.at(ord);
  const auto total = getTotalWordsCnt();
  return {low, high, total};
}

}  // namespace ael::dict
