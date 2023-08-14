#ifndef AEL_IMPL_ESC_DICT_PPM_A_D_DICTIONARY_BASE_HPP
#define AEL_IMPL_ESC_DICT_PPM_A_D_DICTIONARY_BASE_HPP

#include <ael/impl/dictionary/ctx_base.hpp>
#include <ael/impl/dictionary/max_ord_base.hpp>
#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <boost/container/static_vector.hpp>
#include <cstdint>

namespace ael::impl::esc::dict {

constexpr auto ppmadCtxMaxLength = std::uint16_t{16};

////////////////////////////////////////////////////////////////////////////////
class PPMADDictionaryBase
    : protected ael::impl::dict::MaxOrdBase<std::uint64_t>,
      protected ael::impl::dict::CtxBase<std::uint64_t, ppmadCtxMaxLength> {
 public:
  using Ord = MaxOrdBase::Ord;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;

 protected:
  constexpr static std::uint16_t maxCtxLength_ = ppmadCtxMaxLength;
  using SearchCtx_ = boost::container::static_vector<Ord, maxCtxLength_>;

 public:
  PPMADDictionaryBase() = delete;

 protected:
  PPMADDictionaryBase(Ord maxOrd, std::size_t ctxLength)
      : MaxOrdBase(maxOrd), CtxBase(ctxLength) {
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
    return ord == getMaxOrd_();
  }

 protected:
  [[nodiscard]] std::size_t getEscDecoded_() const {
    return escDecoded_;
  }

  void updateEscDecoded_(Ord ord);

  void skipCtxsByEsc_(SearchCtx_& currCtx) const {
    assert(getEscDecoded_() < currCtx.size() && "Checked other cases.");
    currCtx.resize(currCtx.size() - getEscDecoded_());
  }

 private:
  std::size_t escDecoded_{0};
};

}  // namespace ael::impl::esc::dict

#endif  // AEL_IMPL_ESC_DICT_PPM_A_D_DICTIONARY_BASE_HPP
