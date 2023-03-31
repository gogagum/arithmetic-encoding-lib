#include <ael/dictionary/adaptive_a_dictionary.hpp>
#include <boost/range/iterator_range.hpp>

#include "integer_random_access_iterator.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveADictionary::AdaptiveADictionary(Ord maxOrd)
    : impl::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  using UIntIt = ael::impl::IntegerRandomAccessIterator<std::uint64_t>;
  const auto idxs =
      boost::iterator_range<UIntIt>(UIntIt{0}, UIntIt{this->getMaxOrd_()});
  // TODO(gogagum): replace
  // auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
  const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
    return this->getLowerCumulativeCnt_(ord + 1);
  };
  const auto iter =
      std::ranges::upper_bound(idxs, cumulativeCnt, {}, getLowerCumulNumFound_);
  return iter - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  assert(ord < getMaxOrd_() && "ord is out of range");
  const auto ret = getProbabilityStats_(ord);
  this->updateWordCnt_(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getTotalWordsCnt() const -> Count {
  const auto uniqueWordsCnt = this->getTotalWordsUniqueCnt_();
  const auto wordsCnt = this->getRealTotalWordsCnt_();
  if (this->getMaxOrd_() == uniqueWordsCnt) {
    return wordsCnt;
  }
  return (this->getMaxOrd_() - uniqueWordsCnt) * (wordsCnt + 1);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  const auto cumulativeNumFound = this->getRealLowerCumulativeWordCnt_(ord);
  if (this->getMaxOrd_() == this->getTotalWordsUniqueCnt_()) {
    return cumulativeNumFound;
  }
  const auto numUniqueWordsTotal = this->getTotalWordsUniqueCnt_();
  const auto cumulativeUniqueWordsNumFound =
      this->getLowerCumulativeUniqueNumFound_(ord);
  return (this->getMaxOrd_() - numUniqueWordsTotal) * cumulativeNumFound +
         (ord - cumulativeUniqueWordsNumFound);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getWordCnt_(Ord ord) const -> Count {
  const auto totalUniqueWordsCnt = this->getTotalWordsUniqueCnt_();
  if (this->getMaxOrd_() == totalUniqueWordsCnt) {
    return this->getRealWordCnt_(ord);
  }
  return this->getWordUniqueCnt_(ord) == 1
             ? this->getRealWordCnt_(ord) *
                   (this->getMaxOrd_() - totalUniqueWordsCnt)
             : 1;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveADictionary::getProbabilityStats_(Ord ord) const
    -> ProbabilityStats {
  const auto low = getLowerCumulativeCnt_(ord);
  const auto high = low + getWordCnt_(ord);
  const auto total = getTotalWordsCnt();
  assert(high > low);
  assert(total >= high);
  return {low, high, total};
}

}  // namespace ael::dict
