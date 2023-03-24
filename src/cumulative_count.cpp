#include <ael/dictionary/impl/cumulative_count.hpp>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
CumulativeCount::CumulativeCount(Ord maxOrd)
    : _cumulativeCnt(0, maxOrd, 0),
      _totalWordsCnt(0),
      _maxOrd(maxOrd) {}

////////////////////////////////////////////////////////////////////////////////
void CumulativeCount::increaseOrdCount(Ord ord, std::int64_t cntChange) {
    _cumulativeCnt.update(ord, _maxOrd, cntChange);
    _cnt[ord] += cntChange;
    _totalWordsCnt += cntChange;
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeCount::getLowerCumulativeCount(Ord ord) const -> Count {
    if (ord == 0) {
        return 0;
    }
    return getCumulativeCount(ord - 1);
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeCount::getCumulativeCount(Ord ord) const -> Count {
    return _cumulativeCnt.get(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeCount::getCount(Ord ord) const -> Count {
    return _cnt.contains(ord) ? _cnt.at(ord) : Count{0};
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeCount::getTotalWordsCnt() const -> Count {
    return _totalWordsCnt;
}

}  // namespace ael::dict::impl
