#ifndef AEL_IMPL_DICT_CTX_BASE_HPP
#define AEL_IMPL_DICT_CTX_BASE_HPP

#include <boost/container/static_vector.hpp>
#include <cstddef>
#include <cstdint>
#include <deque>

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The CtxBase<OrdT> - context base class.
///
template <typename OrdT, std::uint16_t maxCtxLength>
class CtxBase {
 protected:
  using Ord = OrdT;
  using SearchCtx_ = boost::container::static_vector<Ord, maxCtxLength>;

 protected:
  explicit CtxBase(std::size_t ctxLength) : ctxLength_{ctxLength} {
  }

  void updateCtx_(Ord ord);

  [[nodiscard]] std::size_t getCtxLength_() const {
    return ctxLength_;
  }

  [[nodiscard]] SearchCtx_ getInitSearchCtx_() const {
    return SearchCtx_(ctx_.rbegin(), ctx_.rend());
  }

  template <class CtxWasFound>
  void skipNewCtxs_(SearchCtx_& currCtx, const CtxWasFound& ctxWasFound) const;

 private:
  const std::size_t ctxLength_;
  std::deque<Ord> ctx_{};
};

////////////////////////////////////////////////////////////////////////////////
template <typename OrdT, std::uint16_t maxCtxLength>
void CtxBase<OrdT, maxCtxLength>::updateCtx_(Ord ord) {
  ctx_.push_back(ord);
  if (ctx_.size() > ctxLength_) {
    ctx_.pop_front();
  }
}

////////////////////////////////////////////////////////////////////////////////
template <typename OrdT, std::uint16_t maxCtxLength>
template <class CtxWasFound>
void CtxBase<OrdT, maxCtxLength>::skipNewCtxs_(
    SearchCtx_& currCtx, const CtxWasFound& ctxWasFound) const {
  for (; !currCtx.empty() && !ctxWasFound(currCtx); currCtx.pop_back()) {
    // Skip all empty contexts.
  }
}

}  // namespace ael::impl::dict

#endif