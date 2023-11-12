#ifndef AEL_DICT_ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
#define AEL_DICT_ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP

#include <ael/dictionary/adaptive_d_dictionary.hpp>
#include <ael/impl/dictionary/contextual_dictionary_base_improved.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The adaptive contextual D improved dictionary class
using AdaptiveDContextualDictionaryImproved =
    ael::impl::dict::ContextualDictionaryBaseImproved<AdaptiveDDictionary>;

}  // namespace ael::dict

#endif  // AEL_DICT_ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
