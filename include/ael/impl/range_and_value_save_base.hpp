#ifndef AEL_IMPL_RANGE_AND_VALUE_SAVE_BASE_HPP
#define AEL_IMPL_RANGE_AND_VALUE_SAVE_BASE_HPP

#include <ranges>

#include "range_save_base.hpp"

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The RangeAndValueSaveBase class.
template <class Derived>
class RangeAndValueSaveBase : protected RangeSaveBase {
 protected:
  using WideNum_ = RangeSaveBase::WideNum_;

 protected:
  template <class RC>
  RC::Count calcValue_();

 private:
  Derived& this_();

 protected:
  WideNum_ prevValue_{};

 private:
  bool firstDecode_{true};
};

////////////////////////////////////////////////////////////////////////////////
template <class Derived>
template <class RC>
RC::Count RangeAndValueSaveBase<Derived>::calcValue_() {
  if (firstDecode_) {
    auto ret = typename RC::Count{0};
    for (auto _ : std::ranges::iota_view(std::size_t{0}, RC::numBits)) {
      ret = (ret << 1) + this_().template takeBit_<typename RC::Count>();
    }
    firstDecode_ = false;
    return ret;
  }
  auto ret = impl::multiply_and_divide(prevValue_, WideNum_{RC::total},
                                       prevRange_.total);
  return static_cast<RC::Count>(ret);
}

////////////////////////////////////////////////////////////////////////////////
template <class Derived>
Derived& RangeAndValueSaveBase<Derived>::this_() {
  return *static_cast<Derived*>(this);
}

}  // namespace ael::impl

#endif
