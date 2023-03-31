#include <ael/dictionary/impl/a_d_dictionary_base.hpp>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
ADDictionaryBase::ADDictionaryBase(Ord maxOrd)
    : cumulativeCnt_(maxOrd), cumulativeUniqueCnt_(maxOrd), maxOrd_(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::getRealTotalWordsCnt_() const -> Count {
  return cumulativeCnt_.getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::getRealLowerCumulativeWordCnt_(Ord ord) const -> Count {
  return cumulativeCnt_.getLowerCumulativeCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::getTotalWordsUniqueCnt_() const -> Count {
  return cumulativeUniqueCnt_.getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::getLowerCumulativeUniqueNumFound_(Ord ord) const
    -> Count {
  return cumulativeUniqueCnt_.getLowerCumulativeCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::getRealWordCnt_(Ord ord) const -> Count {
  return cumulativeCnt_.getCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
auto ADDictionaryBase::getWordUniqueCnt_(Ord ord) const -> Count {
  return cumulativeUniqueCnt_.getCount(ord);
}

////////////////////////////////////////////////////////////////////////////////
void ADDictionaryBase::updateWordCnt_(Ord ord, Count cnt) {
  cumulativeCnt_.increaseOrdCount(ord, static_cast<std::int64_t>(cnt));
  cumulativeUniqueCnt_.update(ord);
}

}  // namespace ael::dict::impl
