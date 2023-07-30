#include <ael/impl/dictionary/a_d_dictionary_base.hpp>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
ADDictionaryBase::ADDictionaryBase(Ord maxOrd)
    : cumulativeCnt_(maxOrd), cumulativeUniqueCnt_(maxOrd), maxOrd_(maxOrd) {
}

////////////////////////////////////////////////////////////////////////////////
void ADDictionaryBase::updateWordCnt_(Ord ord, Count cnt) {
  cumulativeCnt_.increaseOrdCount(ord, static_cast<std::int64_t>(cnt));
  cumulativeUniqueCnt_.update(ord);
}

}  // namespace ael::impl::dict
