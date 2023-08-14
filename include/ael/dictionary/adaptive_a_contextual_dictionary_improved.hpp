#ifndef AEL_DICT_ADAPTIVE_A_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
#define AEL_DICT_ADAPTIVE_A_CONTEXTUAL_DICTIONARY_IMPROVED_HPP

#include <ael/dictionary/adaptive_a_dictionary.hpp>
#include <ael/impl/dictionary/contextual_dictionary_base_improved.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The adaptive contextual A improved dictionary class
using AdaptiveAContextualDictionaryImproved =
    ael::impl::dict::ContextualDictionaryBaseImproved<AdaptiveADictionary>;

}  // namespace ael::dict

#endif  // AEL_DICT_ADAPTIVE_A_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
