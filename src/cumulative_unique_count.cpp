#include <ael/dictionary/impl/cumulative_unique_count.hpp>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
CumulativeUniqueCount::CumulativeUniqueCount(Ord maxOrd)
    : cumulativeUniqueCnt_(0, maxOrd, 0), maxOrd_(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
void CumulativeUniqueCount::update(Ord ord) {
  if (!ords_.contains(ord)) {
    cumulativeUniqueCnt_.update(ord, maxOrd_, 1);
    ords_.insert(ord);
  }
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeUniqueCount::getLowerCumulativeCount(Ord ord) const -> Count {
  if (ord == 0) {
    return 0;
  }
  return getCumulativeCount(ord - 1);
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeUniqueCount::getCumulativeCount(Ord ord) const -> Count {
  return cumulativeUniqueCnt_.get(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeUniqueCount::getCount(Ord ord) const -> Count {
  return static_cast<Count>(ords_.contains(ord) ? 1 : 0);
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeUniqueCount::getTotalWordsCnt() const -> Count {
  return ords_.size();
}

}  // namespace ael::dict::impl
