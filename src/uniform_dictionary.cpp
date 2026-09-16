#include <ael/dictionary/uniform_dictionary.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
auto UniformDictionary::getWordOrd(const Count cumulativeNumFound) const -> Ord {
  return cumulativeNumFound;
}

////////////////////////////////////////////////////////////////////////////////
auto UniformDictionary::getProbabilityStats(const Ord ord) -> ProbabilityStats {
  return {
      .low = ord,
      .high = ord + 1,
      .total = maxOrd_,
  };
}

}  // namespace ael::dict
