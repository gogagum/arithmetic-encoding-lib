#ifndef UNIFORM_DICTIONARY_HPP
#define UNIFORM_DICTIONARY_HPP

#include "word_probability_stats.hpp"

#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The UniformDictionary class
///
class UniformDictionary {
public:

    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;

public:

    /**
     * @brief UniformDictionary constructor.
     * @param maxOrd - maximal word order.
     */
    UniformDictionary(Ord maxOrd) : _maxOrd(maxOrd) {}

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return [low, high, total]
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief totalWordsCount
     * @return
     */
    [[nodiscard]] Count getTotalWordsCount() const { return _maxOrd; }

private:
    const Ord _maxOrd;
};

}  // namespace ael::dict

#endif // UNIFORM_DICTIONARY_HPP
