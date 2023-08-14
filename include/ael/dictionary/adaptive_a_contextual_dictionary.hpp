#ifndef AEL_DICT_ADAPTIVE_A_CONTEXTUAL_DICTIONARY_HPP
#define AEL_DICT_ADAPTIVE_A_CONTEXTUAL_DICTIONARY_HPP

#include <ael/dictionary/adaptive_a_dictionary.hpp>
#include <ael/impl/dictionary/contextual_dictionary_base.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The adaptive contextual A dictionary class
using AdaptiveAContextualDictionary =
    ael::impl::dict::ContextualDictionaryBase<AdaptiveADictionary>;

}  // namespace ael::dict

#endif  // AEL_DICT_ADAPTIVE_A_CONTEXTUAL_DICTIONARY_HPP
