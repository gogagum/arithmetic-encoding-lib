#include <ael/dictionary/impl/cumulative_count.hpp>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
CumulativeCount::CumulativeCount(Ord maxOrd)
    : cumulativeCnt_(0, maxOrd, 0), maxOrd_(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
void CumulativeCount::increaseOrdCount(Ord ord, std::int64_t cntChange) {
  cumulativeCnt_.update(ord, maxOrd_, cntChange);
  cnt_[ord] += cntChange;
  totalWordsCnt_ += cntChange;
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeCount::getLowerCumulativeCount(Ord ord) const -> Count {
  return (ord > 0) ? getCumulativeCount(ord - 1) : 0;
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeCount::getCumulativeCount(Ord ord) const -> Count {
  return cumulativeCnt_.get(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeCount::getCount(Ord ord) const -> Count {
  return cnt_.contains(ord) ? cnt_.at(ord) : Count{0};
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeCount::getTotalWordsCnt() const -> Count {
  return totalWordsCnt_;
}

}  // namespace ael::dict::impl
