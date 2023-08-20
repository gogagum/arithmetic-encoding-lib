#ifndef AEL_IMPL_SPAN_EQUALITY
#define AEL_IMPL_SPAN_EQUALITY

#include <algorithm>
#include <span>

namespace ael::impl {
struct SpanEquality {
  template <class T, class U>
  [[nodiscard]] bool operator()(const std::span<T>& l,
                                const std::span<U>& r) const {
    return std::ranges::equal(l, r);
  }
};
}  // namespace ael::impl

#endif  // AEL_IMPL_SPAN_EQUALITY