#include <ael/impl/dictionary/cumulative_unique_count.hpp>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
void CumulativeUniqueCount::update(Ord ord) {
  if (!ords_.contains(ord)) {
    cumulativeUniqueCnt_.update(ord, maxOrd_, 1);
    ords_.insert(ord);
  }
}

}  // namespace ael::impl::dict
