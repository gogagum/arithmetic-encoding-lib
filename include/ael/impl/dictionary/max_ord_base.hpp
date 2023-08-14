#ifndef MAX_ORD_BASE_HPP
#define MAX_ORD_BASE_HPP

#include <ranges>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The max ord functionality class.
///
template <class OrdT>
class MaxOrdBase {
 protected:
  using Ord = OrdT;

 protected:
  explicit MaxOrdBase(Ord maxOrd) : maxOrd_(maxOrd) {
  }

  [[nodiscard]] Ord getMaxOrd_() const {
    return maxOrd_;
  }

  [[nodiscard]] std::ranges::iota_view<Ord, Ord> getOrdRng_() const {
    return std::ranges::iota_view(Ord{0}, maxOrd_);
  }

 private:
  Ord maxOrd_;
};

}  // namespace ael::impl::dict

#endif