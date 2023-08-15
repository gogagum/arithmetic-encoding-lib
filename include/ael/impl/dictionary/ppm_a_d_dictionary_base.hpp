#ifndef AEL_IMPL_DICT_PPM_A_D_DICTIONARY_BASE_HPP
#define AEL_IMPL_DICT_PPM_A_D_DICTIONARY_BASE_HPP

#include <ael/impl/dictionary/ctx_base.hpp>
#include <ael/impl/dictionary/max_ord_base.hpp>
#include <cstdint>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMA and PPMD dictionary base class.
///
template <class DictT, std::uint16_t maxCtxLength>
class PPMADDictionaryBase : protected MaxOrdBase<std::uint64_t>,
                            protected CtxBase<DictT, std::uint64_t, maxCtxLength> {
 public:
  using Ord = MaxOrdBase::Ord;

 protected:
  using SearchCtx_ = CtxBase<DictT, std::uint64_t, maxCtxLength>::SearchCtx_;

 public:
  PPMADDictionaryBase() = delete;

 protected:
  explicit PPMADDictionaryBase(Ord maxOrd, std::size_t ctxLength)
      : MaxOrdBase(maxOrd), CtxBase<DictT, std::uint64_t, maxCtxLength>(ctxLength) {
  }
};

}  // namespace ael::impl::dict

#endif  // AEL_IMPL_DICT_PPM_A_D_DICTIONARY_BASE_HPP