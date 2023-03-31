#include <ael/dictionary/impl/a_d_dictionary_base.hpp>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
ADDictionaryBase::ADDictionaryBase(Ord maxOrd)
    : cumulativeCnt_(maxOrd), cumulativeUniqueCnt_(maxOrd), maxOrd_(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getRealTotalWordsCnt() const -> Count {
  return cumulativeCnt_.getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getRealLowerCumulativeWordCnt(Ord ord) const -> Count {
  return cumulativeCnt_.getLowerCumulativeCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getTotalWordsUniqueCnt() const -> Count {
  return cumulativeUniqueCnt_.getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getLowerCumulativeUniqueNumFound(Ord ord) const
    -> Count {
  return cumulativeUniqueCnt_.getLowerCumulativeCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getRealWordCnt(Ord ord) const -> Count {
  return cumulativeCnt_.getCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::_getWordUniqueCnt(Ord ord) const -> Count {
  return cumulativeUniqueCnt_.getCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
void ADDictionaryBase::_updateWordCnt(Ord ord, Count cnt) {
  cumulativeCnt_.increaseOrdCount(ord, static_cast<std::int64_t>(cnt));
  cumulativeUniqueCnt_.update(ord);
}

}  // namespace ael::dict::impl
