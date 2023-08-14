#ifndef AEL_DICT_ADAPTIVE_D_CONTECTUAL_DICTIONARY_HPP
#define AEL_DICT_ADAPTIVE_D_CONTECTUAL_DICTIONARY_HPP

#include <ael/dictionary/adaptive_d_dictionary.hpp>
#include <ael/impl/dictionary/contextual_dictionary_base.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
// \brief The adaptive contextual D dictionary class
using AdaptiveDContextualDictionary =
    ael::impl::dict::ContextualDictionaryBase<AdaptiveDDictionary>;

}  // namespace ael::dict

#endif  // AEL_DICT_ADAPTIVE_D_CONTECTUAL_DICTIONARY_HPP
