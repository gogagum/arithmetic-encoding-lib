#ifndef ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
#define ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP

#include <ael/dictionary/adaptive_d_dictionary.hpp>
#include <ael/impl/dictionary/contectual_dictionary_base_improved.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The adaptive contextual D improved dictionary class
using AdaptiveDContextualDictionaryImproved =
    ael::impl::dict::ContextualDictionaryBaseImproved<AdaptiveDDictionary>;

}  // namespace ael::dict

#endif  // ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
