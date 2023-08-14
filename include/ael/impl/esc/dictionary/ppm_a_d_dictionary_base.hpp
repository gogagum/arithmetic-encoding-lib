#ifndef IMPL_ESC_PPM_A_D_DICTIONARY_BASE_HPP
#define IMPL_ESC_PPM_A_D_DICTIONARY_BASE_HPP

#include <ael/impl/dictionary/word_probability_stats.hpp>
#include <boost/container/static_vector.hpp>
#include <cstdint>
#include <deque>

#include "ael/impl/dictionary/max_ord_base.hpp"

namespace ael::impl::esc::dict {

////////////////////////////////////////////////////////////////////////////////
class PPMADDictionaryBase
    : protected ael::impl::dict::MaxOrdBase<std::uint64_t> {
 public:
  using Ord = MaxOrdBase::Ord;
  using Count = std::uint64_t;
  using ProbabilityStats = ael::impl::dict::WordProbabilityStats<Count>;

 protected:
  constexpr static std::uint16_t maxCtxLength_ = 16;
  using SearchCtx_ = boost::container::static_vector<Ord, maxCtxLength_>;

 public:
  PPMADDictionaryBase() = delete;

 protected:
  PPMADDictionaryBase(Ord maxOrd, std::size_t ctxLength)
      : MaxOrdBase(maxOrd), ctxLength_(ctxLength) {
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

  [[nodiscard]] const std::deque<Ord>& getCtx_() const {
    return ctx_;
  }

  void updateCtx_(Ord ord);

  void skipNewCtxs_(SearchCtx_& currCtx) const;

  void skipCtxsByEsc_(
      SearchCtx_& currCtx) const {  // TODO(gogagum): move to base
    assert(getEscDecoded_() < currCtx.size() && "Checked other cases.");
    currCtx.resize(currCtx.size() - getEscDecoded_());
  }

 private:
  const std::size_t ctxLength_{};
  const Ord maxOrd_{};
  std::deque<Ord> ctx_{};
  std::size_t escDecoded_{0};
};

}  // namespace ael::impl::esc::dict

#endif
