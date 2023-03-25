#ifndef PPMD_DICTIONARY_HPP
#define PPMD_DICTIONARY_HPP

#include "ael/dictionary/impl/cumulative_count.hpp"
#include "ael/dictionary/impl/cumulative_unique_count.hpp"
#include "impl/word_probability_stats.hpp"

#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <deque>
#include <unordered_map>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMDDictionary - ppmd probability model.
///
class PPMDDictionary {
public:
    using Ord = std::uint64_t;
    using Count = boost::multiprecision::uint256_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
    constexpr const static std::uint16_t countNumBits = 240;
private:
    constexpr const static std::uint16_t _maxSeqLenLog2 = 40;
public:

    /**
     * PPMD dictionary constructor.
     * @param maxOrd - maximal order. 
     */
    PPMDDictionary(Ord maxOrd, std::size_t ctxLength);
    
    /**
     * @brief getWordOrd - get word order index by cumulative count.
     * @param cumulativeNumFound search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Ord getWordOrd(const Count& cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats - get probability stats and update.
     * @param ord - order of a word.
     * @return [low, high, total]
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief getTotalWordsCount - get total words count estimation.
     * @return total words count estimation
     */
    [[nodiscard]] Count getTotalWordsCnt() const;

private:
    using SearchCtx_ = boost::container::static_vector<Ord, 16>;
    using SearchCtxHash_ = boost::hash<SearchCtx_>;
    struct CtxCell_ {
        impl::CumulativeCount cnt;
        impl::CumulativeUniqueCount uniqueCnt;
    };
    using CtxCountMapping_ = std::unordered_map<
        SearchCtx_,
        CtxCell_,
        SearchCtxHash_
    >;
private:

    Count _getLowerCumulativeCnt(Ord ord) const;

    ProbabilityStats _getProbabilityStats(Ord ord) const;

    void _updateWordCnt(Ord ord, impl::CumulativeCount::Count cnt);

    SearchCtx_ _getSearchCtxEmptySkipped() const;

private:
    std::size_t _maxOrd;
    CtxCell_ _zeroCtxCell;
    std::deque<Ord> _ctx;
    CtxCountMapping_ _ctxInfo;
    const std::size_t _ctxLength;
};

}  // namespace ael::dict

#endif  // PPMD_DICTIONARY_HPP
