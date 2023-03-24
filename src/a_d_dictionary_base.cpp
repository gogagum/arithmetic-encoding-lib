#include <ael/dictionary/impl/a_d_dictionary_base.hpp>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
ADDictionaryBase::ADDictionaryBase(Ord maxOrd)
    : _cumulativeCnt(maxOrd),
      _cumulativeUniqueCnt(maxOrd),
      _maxOrd(maxOrd) {}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getRealTotalWordsCnt() const -> Count {
    return _cumulativeCnt.getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getRealLowerCumulativeWordCnt(Ord ord) const -> Count {
    return _cumulativeCnt.getLowerCumulativeCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getTotalWordsUniqueCnt() const -> Count {
    return _cumulativeUniqueCnt.getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getLowerCumulativeUniqueNumFound(
        Ord ord) const -> Count {
    return _cumulativeUniqueCnt.getLowerCumulativeCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getRealWordCnt(Ord ord) const -> Count {
    return _cumulativeCnt.getCount(ord); 
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getWordUniqueCnt(Ord ord) const -> Count {
    return _cumulativeUniqueCnt.getCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
void ADDictionaryBase::_updateWordCnt(Ord ord, Count cnt) {
    _cumulativeCnt.increaseOrdCount(ord, cnt);
    _cumulativeUniqueCnt.update(ord);
}

}  // namespace ael::dict::impl
