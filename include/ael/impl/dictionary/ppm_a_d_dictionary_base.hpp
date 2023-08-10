#ifndef PPM_A_D_DICTIONARY_BASE_HPP
#define PPM_A_D_DICTIONARY_BASE_HPP

#include <cstdint>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMA and PPMD dictionary base class.
///
class PPMADDictionaryBase {
 public:
  using Ord = std::uint64_t;

 public:
  PPMADDictionaryBase() = delete;

 protected:
  explicit PPMADDictionaryBase(Ord maxOrd) : maxOrd_{maxOrd} {
  }

  [[nodiscard]] Ord getMaxOrd_() const {
    return maxOrd_;
  }

 private:
  const Ord maxOrd_;
};

}  // namespace ael::impl::dict

#endif