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

////////////////////////////////////////////////////////////////////////////////
void PPMADDictionaryBase::updateCtx_(Ord ord) {
  ctx_.push_back(ord);
  if (ctx_.size() > ctxLength_) {
    ctx_.pop_front();
  }
}

}  // namespace ael::impl::esc::dict
