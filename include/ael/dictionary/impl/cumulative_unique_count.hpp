#ifndef CUMULATIVE_UNIQUE_COUNT_HPP
#define CUMULATIVE_UNIQUE_COUNT_HPP

#include <cstdint>
#include <dst/dynamic_segment_tree.hpp>
#include <unordered_set>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The CumulativeUniqueCount class.
///
class CumulativeUniqueCount {
public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
public:

    /**
     * @brief CumulativeCount constructor.
     * @param maxOrd - maximalOrder;
     */
    CumulativeUniqueCount(Ord maxOrd);

    /**
     * @brief update - update ord info.
     * @param ord - where to increase count.
     */
    void update(Ord ord);

    /**
     * @brief getLowerCumulativeCount - get lower cumulative count from zero
     * to given ord not including it.
     * @param ord - order index of a checked word.
     * @return cumulative count.
     */
    Count getLowerCumulativeCount(Ord ord) const;

    /**
     * @brief getCumulativeCount - get cumulative count from zero
     * to given ord including it.
     * @param ord - order index of a checked word.
     * @return cumulative count.
     */
    Count getCumulativeCount(Ord ord) const;

    /**
     * @brief get count of a word.
     * @param ord - order index of a word.
     * @return word count.
     */
    Count getCount(Ord ord) const;

    /**
     * @brief get total words count.
     * @return total words count.
     */
    Count getTotalWordsCnt() const;

private:
    using _DST =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;
private:
    _DST _cumulativeUniqueCnt;
    std::unordered_set<Ord> _ords;
    const Ord _maxOrd;
};

}  // namespace ael::dict::impl

#endif  // CUMULATIVE_COUNT_HPP
