#include <ael/dictionary/impl/cumulative_unique_count.hpp>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
CumulativeUniqueCount::CumulativeUniqueCount(Ord maxOrd)
    : _cumulativeUniqueCnt(0, maxOrd, 0),
      _maxOrd(maxOrd) {}

////////////////////////////////////////////////////////////////////////////////
void CumulativeUniqueCount::update(Ord ord) {
    if (!_ords.contains(ord)) {
        _cumulativeUniqueCnt.update(ord, _maxOrd, 1);
        _ords.insert(ord);
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
    return _cumulativeUniqueCnt.get(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeUniqueCount::getCount(Ord ord) const -> Count {
    return static_cast<Count>(_ords.contains(ord) ? 1 : 0);
}

////////////////////////////////////////////////////////////////////////////////
auto CumulativeUniqueCount::getTotalWordsCnt() const -> Count {
    return _ords.size();
}

}  // namespace ael::dict::impl
