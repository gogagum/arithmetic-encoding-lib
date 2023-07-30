#include <ael/impl/dictionary/cumulative_count.hpp>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
void CumulativeCount::increaseOrdCount(Ord ord, std::int64_t cntChange) {
  cumulativeCnt_.update(ord, maxOrd_, cntChange);
  cnt_[ord] += cntChange;
  totalWordsCnt_ += cntChange;
}

}  // namespace ael::impl::dict
