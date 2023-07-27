#ifndef ADAPTIVE_D_CONTECTUAL_DICTIONARY_HPP
#define ADAPTIVE_D_CONTECTUAL_DICTIONARY_HPP

#include "adaptive_d_dictionary.hpp"
#include <ael/impl/dictionary/contextual_dictionary_base.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
// \brief The adaptive contextual D dictionary class
using AdaptiveDContextualDictionary =
    ael::impl::dict::ContextualDictionaryBase<AdaptiveDDictionary>;

}  // namespace ael::dict

#endif  // ADAPTIVE_D_CONTECTUAL_DICTIONARY_HPP
