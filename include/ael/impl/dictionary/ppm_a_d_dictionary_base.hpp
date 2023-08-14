#ifndef PPM_A_D_DICTIONARY_BASE_HPP
#define PPM_A_D_DICTIONARY_BASE_HPP

#include <cstdint>
#include "ael/impl/dictionary/max_ord_base.hpp"

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

#endif