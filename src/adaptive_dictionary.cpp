#include <ael/dictionary/adaptive_dictionary.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <ranges>

namespace ael::dict {

using boost::lambda::_1;
using boost::lambda::bind;
using std::ranges::upper_bound;

////////////////////////////////////////////////////////////////////////////////
AdaptiveDictionary::AdaptiveDictionary(ConstructInfo constructInfo)
    : ael::impl::dict::AdaptiveDictionaryBase<Count>(constructInfo.maxOrd,
                                                     constructInfo.maxOrd),
      ratio_(constructInfo.ratio),
      maxOrder_(constructInfo.maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getWordOrd(Count cumulativeCnt) const -> Ord {
  return *upper_bound(getOrdRng_(), cumulativeCnt, {},
                      bind(&This_::getLowerCumulativeCnt_, this, _1 + 1));
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
  const auto low = getLowerCumulativeCnt_(ord);
  const auto high = low + getRealWordCnt_(ord) * ratio_ + 1;
  const auto total = getTotalWordsCnt();
  updateWordCnt_(ord);
  return {low, high, total};
}

////////////////////////////////////////////////////////////////////////////////
void AdaptiveDictionary::updateWordCnt_(Ord ord) {
  this->changeRealCumulativeWordCnt_(ord, 1);
  this->changeRealWordCnt_(ord, 1);
  this->changeRealTotalWordsCnt_(1);
}

////////////////////////////////////////////////////////////////////////////////
auto AdaptiveDictionary::getLowerCumulativeCnt_(Ord ord) const -> Count {
  return (ord > Ord{0}) ? ord + getRealCumulativeCnt_(ord - 1) * ratio_
                        : Count{0};
}

}  // namespace ael::dict
