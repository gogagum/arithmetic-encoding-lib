#ifndef AEL_IMPL_RANGE_SAVE_BASE_HPP
#define AEL_IMPL_RANGE_SAVE_BASE_HPP

#include "multiply_and_divide.hpp"

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The RangeSaveBase class.
class RangeSaveBase {
 protected:
  template <class RC>
  RC::Range calcRange_();

 protected:
  using WideNum_ = boost::multiprecision::uint256_t;
  struct TmpRange_ {
    WideNum_ low;
    WideNum_ high;
    WideNum_ total;
  };

 protected:
  TmpRange_ prevRange_{0, 1, 1};
};

////////////////////////////////////////////////////////////////////////////////
template <class RC>
RC::Range RangeSaveBase::calcRange_() {
  auto retLow = impl::multiply_and_divide(prevRange_.low, WideNum_{RC::total},
                                          prevRange_.total);
  auto retHigh = impl::multiply_decrease_and_divide(
                     prevRange_.high, WideNum_{RC::total}, prevRange_.total) +
                 1;
  return {static_cast<RC::Count>(retLow), static_cast<RC::Count>(retHigh)};
}

}  // namespace ael::impl

#endif  // AEL_IMPL_RANGE_SAVE_BASE_HPP
