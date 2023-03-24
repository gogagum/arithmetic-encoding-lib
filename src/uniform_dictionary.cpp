#include <ael/dictionary/uniform_dictionary.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
auto UniformDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    return cumulativeNumFound;
}

////////////////////////////////////////////////////////////////////////////////
auto UniformDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    return { ord, ord + 1, _maxOrd };
}

}  // namespace ael::dict
