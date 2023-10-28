#ifndef AEL_IMPL_CONTEXT_BUFFER_HPP
#define AEL_IMPL_CONTEXT_BUFFER_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <span>
#include <stdexcept>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// Context<OrdT, size> - context class.
///
template <std::integral OrdT, std::size_t size>
class ContextBuffer {
 public:
  explicit ContextBuffer(std::size_t ctxLength = size) : ctxLength_{ctxLength} {
    if (ctxLength_ > size) {
      throw std::logic_error("Too big context length.");
    }
  }

  /**
   * @brief Get context as span with length, which is not longer than `size`.
   *
   * @return std::span<const OrdT> context, last added elements.
   */
  [[nodiscard]] std::span<const OrdT> getCtx() const;

  /**
   * @brief update context.
   *
   * @param ord added word index
   */
  void add(OrdT ord);

 private:
  std::array<OrdT, size * 2> buffer_{};
  std::size_t offset_{};
  std::size_t ctxLength_;
};

////////////////////////////////////////////////////////////////////////////////
template <std::integral OrdT, std::size_t size>
auto ContextBuffer<OrdT, size>::getCtx() const -> std::span<const OrdT> {
  if (offset_ < ctxLength_) [[unlikely]] {
    return std::span(buffer_.begin(), offset_);
  }
  auto beginOffset = (0 == ctxLength_) ? 0 : (offset_ % ctxLength_);
  return std::span(buffer_.begin() + beginOffset, ctxLength_);
}

////////////////////////////////////////////////////////////////////////////////
template <std::integral OrdT, std::size_t size>
void ContextBuffer<OrdT, size>::add(OrdT ord) {
  if (0 != ctxLength_) [[unlikely]] {
    buffer_.at(offset_ % ctxLength_) = ord;
    buffer_.at(offset_ % ctxLength_ + ctxLength_) = ord;
    ++offset_;
  }
}

}  // namespace ael::impl

#endif  // AEL_IMPL_CONTEXT_BUFFER_HPP
