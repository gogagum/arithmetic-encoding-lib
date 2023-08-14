#ifndef AEL_IMPL_ESC_DICT_A_D_DICTIONARY_BASE_HPP
#define AEL_IMPL_ESC_DICT_A_D_DICTIONARY_BASE_HPP

#include <ael/impl/dictionary/a_d_dictionary_base.hpp>

namespace ael::impl::esc::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The esc base A/D dictionary base class.
///
class ADDictionaryBase : protected ael::impl::dict::ADDictionaryBase {
 public:
  using ael::impl::dict::ADDictionaryBase::ADDictionaryBase;

  [[nodiscard]] bool isEsc(Ord ord) const {
    return ord == getMaxOrd_();
  }
};

}  // namespace ael::impl::esc::dict

#endif  // AEL_IMPL_ESC_DICT_A_D_DICTIONARY_BASE_HPP
