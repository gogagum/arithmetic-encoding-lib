#include <ael/dictionary/static_dictionary.hpp>

#include <algorithm>
#include <boost/range/irange.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
auto StaticDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    auto it = std::ranges::upper_bound(_cumulativeNumFound, cumulativeNumFound);
    return it - _cumulativeNumFound.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto StaticDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = _getHigherCumulativeCnt(ord);
    return { low, high, *_cumulativeNumFound.rbegin() };
}

////////////////////////////////////////////////////////////////////////////////
auto StaticDictionary::_getLowerCumulativeCnt(Ord ord) const -> Count {
    return ord != 0 ? _cumulativeNumFound[ord - 1] : 0;
}

}  // namespace ael::dict
