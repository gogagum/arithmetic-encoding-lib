#ifndef IMPL_ESC_PPM_A_D_DICTIONARY_BASE_HPP
#define IMPL_ESC_PPM_A_D_DICTIONARY_BASE_HPP

#include <cstdint>
#include <ael/impl/dictionary/word_probability_stats.hpp>

namespace ael::impl::esc::dict {

////////////////////////////////////////////////////////////////////////////////
class PPMADDictionaryBase {
 public:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;

 protected:
  explicit PPMADDictionaryBase(Ord maxOrd) : maxOrd_{maxOrd} {
  }

 public:
  /**
   * @brief Check if word is esc symbol.
   *
   * @param ord word index.
   * @return true if word is an esc-symbol.
   * @return false if word is not an esc symbol.
   */
  [[nodiscard]] bool isEsc(Ord ord) const {
    return ord == maxOrd_;
  }

 protected:
  [[nodiscard]] Ord getMaxOrd_() const {
    return maxOrd_;
  }

  [[nodiscard]] std::size_t getEscDecoded_() const {
    return escDecoded_;
  }

  void updateEscDecoded_(Ord ord) const;

 private:
  const Ord maxOrd_;
  mutable std::size_t escDecoded_{0};
};

}  // namespace ael::impl::esc::dict

#endif
