#ifndef AEL_IMPL_DICT_PPM_A_D_DICTIONARY_BASE_HPP
#define AEL_IMPL_DICT_PPM_A_D_DICTIONARY_BASE_HPP

#include <ael/impl/dictionary/max_ord_base.hpp>
#include <cstdint>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMA and PPMD dictionary base class.
///
class PPMADDictionaryBase : protected MaxOrdBase<std::uint64_t> {
 public:
  using Ord = MaxOrdBase::Ord;

 public:
  PPMADDictionaryBase() = delete;

 protected:
  explicit PPMADDictionaryBase(Ord maxOrd) : MaxOrdBase(maxOrd) {
  }
};

}  // namespace ael::impl::dict

#endif  // AEL_IMPL_DICT_PPM_A_D_DICTIONARY_BASE_HPP