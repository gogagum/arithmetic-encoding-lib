#ifndef ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
#define ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP

#include "impl/contectual_dictionary_base_improved.hpp"
#include "adaptive_d_dictionary.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The adaptive contextual D improved dictionary class
using AdaptiveDContextualDictionaryImproved =
    impl::ContextualDictionaryBaseImproved<AdaptiveDDictionary>;

}  // namespace ael::dict

#endif  // ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
