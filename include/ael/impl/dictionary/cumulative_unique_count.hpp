#ifndef CUMULATIVE_UNIQUE_COUNT_HPP
#define CUMULATIVE_UNIQUE_COUNT_HPP

#include <cstdint>
#include <dst/dynamic_segment_tree.hpp>
#include <unordered_set>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The CumulativeUniqueCount class.
///
class CumulativeUniqueCount {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;

 public:
  CumulativeUniqueCount() = delete;

  /**
   * @brief CumulativeCount constructor.
   * @param maxOrd - maximalOrder;
   */
  explicit CumulativeUniqueCount(Ord maxOrd)
      : cumulativeUniqueCnt_{0, maxOrd, 0}, maxOrd_(maxOrd) {
  }

  /**
   * @brief update - update ord info.
   * @param ord - where to increase count.
   */
  void update(Ord ord);

  /**
   * @brief getLowerCumulativeCnt - get lower cumulative count from zero
   * to given ord not including it.
   * @param ord - order index of a checked word.
   * @return cumulative count.
   */
  [[nodiscard]] Count getLowerCumulativeCnt(Ord ord) const {
    return (ord == 0) ? 0 : getCumulativeCount_(ord - 1);
  }

  /**
   * @brief get count of a word.
   * @param ord - order index of a word.
   * @return word count.
   */
  [[nodiscard]] Count getCount(Ord ord) const {
    return static_cast<Count>(ords_.contains(ord) ? 1 : 0);
  }

  /**
   * @brief get total words count.
   * @return total words count.
   */
  [[nodiscard]] Count getTotalWordsCnt() const {
    return ords_.size();
  }

 protected:
  [[nodiscard]] Ord getmaxOrd_() const {
    return maxOrd_;
  }

  /**
   * @brief getCumulativeCount - get cumulative count from zero
   * to given ord including it.
   * @param ord - order index of a checked word.
   * @return cumulative count.
   */
  [[nodiscard]] Count getCumulativeCount_(Ord ord) const {
    return cumulativeUniqueCnt_.get(ord);
  }

 private:
  using DST_ =
      dst::DynamicSegmentTree<Ord, Count, void, dst::NoRangeGetOp,
                              dst::NoRangeGetOp, std::plus<void>, std::int64_t>;

 private:
  DST_ cumulativeUniqueCnt_;
  std::unordered_set<Ord> ords_{};
  const Ord maxOrd_;
};

}  // namespace ael::impl::dict

#endif  // CUMULATIVE_COUNT_HPP
