#include <ael/impl/dictionary/a_d_dictionary_base.hpp>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
ADDictionaryBase::ADDictionaryBase(const Ord maxOrd)
    : MaxOrdBase(maxOrd), cumulativeCnt_(maxOrd), cumulativeUniqueCnt_(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
void ADDictionaryBase::updateWordCnt_(const Ord ord, Count cnt) {
  cumulativeCnt_.increaseOrdCount(ord, static_cast<std::int64_t>(cnt));
  cumulativeUniqueCnt_.update(ord);
}

}  // namespace ael::impl::dict
