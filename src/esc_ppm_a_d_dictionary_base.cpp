#include <ael/impl/esc/dictionary/ppm_a_d_dictionary_base.hpp>

namespace ael::impl::esc::dict {

////////////////////////////////////////////////////////////////////////////////
void PPMADDictionaryBase::updateEscDecoded_(Ord ord) {
  if (isEsc(ord)) {
    ++escDecoded_;
  } else {
    escDecoded_ = 0;
  }
}

}  // namespace ael::impl::esc::dict
