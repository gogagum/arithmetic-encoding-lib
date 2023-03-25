#ifndef WORD_PROBABILITY_STATS_HPP
#define WORD_PROBABILITY_STATS_HPP

#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ProbabilityStats class
///
template <class CountT>
struct WordProbabilityStats {
    CountT low;
    CountT high;
    CountT total;
};

}  // namespace ael::dict

#endif // WORD_PROBABILITY_STATS_HPP
