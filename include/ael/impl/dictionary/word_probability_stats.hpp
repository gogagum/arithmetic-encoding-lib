#ifndef AEL_IMPL_DICT_WORD_PROBABILITY_STATS_HPP
#define AEL_IMPL_DICT_WORD_PROBABILITY_STATS_HPP

#include <cstdint>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ProbabilityStats class
///
template <class CountT>
struct WordProbabilityStats {
  CountT low;
  CountT high;
  CountT total;
};

}  // namespace ael::impl::dict

#endif  // AEL_IMPL_DICT_WORD_PROBABILITY_STATS_HPP
