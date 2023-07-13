#include <ael/dictionary/adaptive_d_dictionary.hpp>
#include <ranges>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
AdaptiveDDictionary::AdaptiveDDictionary(Ord maxOrd)
    : impl::ADDictionaryBase(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
  const auto idxs = std::ranges::iota_view(Ord{0}, this->getMaxOrd_());
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
  this->updateWordCnt_(ord, 1);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getTotalWordsCnt() const -> Count {
  const auto totalWordsCnt = this->getRealTotalWordsCnt_();
  if (totalWordsCnt == 0) {
    return this->getMaxOrd_();
  }
  const auto totalWordsUniqueCnt = this->getTotalWordsUniqueCnt_();
  if (totalWordsUniqueCnt == this->getMaxOrd_()) {
    return totalWordsCnt;
  }
  return 2 * (this->getMaxOrd_() - totalWordsUniqueCnt) * totalWordsCnt;
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  if (this->getRealTotalWordsCnt_() == 0) {
    return ord;
  }
  const auto totalUniqueWordsCnt = this->getTotalWordsUniqueCnt_();
  const auto cumulativeWordsCnt = this->getRealLowerCumulativeWordCnt_(ord);
  if (totalUniqueWordsCnt == this->getMaxOrd_()) {
    return cumulativeWordsCnt;
  }
  return (this->getMaxOrd_() - totalUniqueWordsCnt) * 2 * cumulativeWordsCnt +
         ord * totalUniqueWordsCnt -
         this->getMaxOrd_() * this->getLowerCumulativeUniqueNumFound_(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDDictionary::getWordCnt_(Ord ord) const -> Count {
  if (this->getRealTotalWordsCnt_() == 0) {
    return 1;
  }
  const auto totalUniqueWordsCount = this->getTotalWordsUniqueCnt_();
  const auto realWordCnt = this->getRealWordCnt_(ord);
  if (totalUniqueWordsCount == this->getMaxOrd_()) {
    return realWordCnt;
  }
  return (this->getMaxOrd_() - totalUniqueWordsCount) * 2 * realWordCnt +
         totalUniqueWordsCount - this->getMaxOrd_() * getWordUniqueCnt_(ord);
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
