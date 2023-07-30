#ifndef CUMULATIVE_COUNT_HPP
#define CUMULATIVE_COUNT_HPP

#include <cstdint>
#include <dst/dynamic_segment_tree.hpp>
#include <unordered_map>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The CumulativeCount class.
///
class CumulativeCount {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;

 public:
  /**
   * @brief CumulativeCount constructor.
   * @param maxOrd - maximalOrder;
   */
  explicit CumulativeCount(Ord maxOrd)
      : cumulativeCnt_(0, maxOrd, 0), maxOrd_(maxOrd) {
  }

  /**
   * @brief increaseOrdCount - increase one word count.
   * @param ord - where to increase count.
   */
  void increaseOrdCount(Ord ord, std::int64_t cntChange);

  /**
   * @brief getLowerCumulativeCount - lower cumulative count getter.
   * @param ord - order of a word.
   * @return lower cumulative count.
   */
  [[nodiscard]] Count getLowerCumulativeCount(Ord ord) const {
    return (ord > 0) ? getCumulativeCount(ord - 1) : 0;
  }

  /**
   * @brief getCumulativeCount - get cumulative count from zero
   * to given ord including it.
   * @param ord - order index of a checked word.
   * @return cumulative count.
   */
  [[nodiscard]] Count getCumulativeCount(Ord ord) const {
    return cumulativeCnt_.get(ord);
  }

  /**
   * @brief get count of a word.
   * @param ord - order index of a word.
   * @return word count.
   */
  [[nodiscard]] Count getCount(Ord ord) const {
    return cnt_.contains(ord) ? cnt_.at(ord) : Count{0};
  }

  /**
   * @brief get total words count.
   * @return total words count.
   */
  [[nodiscard]] Count getTotalWordsCnt() const {
    return totalWordsCnt_;
  }

 private:
  using DST_ =
      dst::DynamicSegmentTree<Ord, Count, void, dst::NoRangeGetOp,
                              dst::NoRangeGetOp, std::plus<void>, std::int64_t>;

 private:
  DST_ cumulativeCnt_;
  std::unordered_map<Ord, Count> cnt_;
  Count totalWordsCnt_{0};
  const Ord maxOrd_;
};

}  // namespace ael::impl::dict

#endif  // CUMULATIVE_COUNT_HPP
