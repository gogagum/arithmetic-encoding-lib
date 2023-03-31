#ifndef ADAPTIVE_A_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
#define ADAPTIVE_A_CONTEXTUAL_DICTIONARY_IMPROVED_HPP

#include <cstdint>
#include <stdexcept>
#include <unordered_map>

#include "adaptive_a_dictionary.hpp"
#include "impl/contectual_dictionary_base_improved.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The adaptive contextual A improved dictionary class
using AdaptiveAContextualDictionaryImproved =
    impl::ContextualDictionaryBaseImproved<AdaptiveADictionary>;

}  // namespace ael::dict

#endif  // ADAPTIVE_A_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
