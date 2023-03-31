#ifndef ADAPTIVE_A_CONTEXTUAL_DICTIONARY_HPP
#define ADAPTIVE_A_CONTEXTUAL_DICTIONARY_HPP

#include "adaptive_a_dictionary.hpp"
#include "impl/contextual_dictionary_base.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The adaptive contextual A dictionary class
using AdaptiveAContextualDictionary =
    impl::ContextualDictionaryBase<AdaptiveADictionary>;

}  // namespace ael::dict

#endif  // ADAPTIVE_A_CONTEXTUAL_DICTIONARY_HPP
