#ifndef AEL_IMPL_BITS_ITERATOR_HPP
#define AEL_IMPL_BITS_ITERATOR_HPP

#include <array>
#include <cstddef>
#include <cstdint>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsIterator class
///
template <class T>
class BitsIterator {
 public:
  constexpr const static std::size_t bitsTSize = sizeof(T) * 8;

 public:
  //////////////////////////////////////////////////////////////////////////////
  explicit BitsIterator(const T& iterated, std::ptrdiff_t offset = bitsTSize)
      : iterated_(&iterated), offset_(offset) {
  }
  //////////////////////////////////////////////////////////////////////////////
  BitsIterator(const BitsIterator&) = default;
  //////////////////////////////////////////////////////////////////////////////
  BitsIterator(BitsIterator&&) noexcept = default;
  //////////////////////////////////////////////////////////////////////////////
  BitsIterator& operator=(const BitsIterator&) = default;
  //////////////////////////////////////////////////////////////////////////////
  BitsIterator& operator=(BitsIterator&&) noexcept = default;

 private:
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::byte getByte_() const {
    const auto& bytes =
        *static_cast<const std::array<const std::byte, sizeof(T)>*>(iterated_);
    return bytes[getByteIdx_()];  // NOLINT
  }
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::size_t getByteIdx_() const {
    constexpr const std::size_t bitsInByte = 8;
    return sizeof(T) - offset_ / bitsInByte - 1;
  }
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::uint8_t getInByteIdx_() const {
    constexpr std::uint8_t bitsInByte = 8;
    return bitsInByte - (offset_ % bitsInByte) - 1;
  }

 public:
  // NOLINTBEGIN(readability-identifier-naming)
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::ptrdiff_t operator-(const BitsIterator<T>& rhs) const {
    return rhs.offset_ - offset_;
  }
  //////////////////////////////////////////////////////////////////////////////
  void advance(std::ptrdiff_t n) {
    offset_ += n;
  }
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool operator*() const {
    return ((getByte_() >> getInByteIdx_()) & std::byte{1}) != std::byte{};
  }
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool operator==(const BitsIterator<T>& other) const {
    return offset_ == other.offset_;
  }
  //////////////////////////////////////////////////////////////////////////////
  BitsIterator<T>& operator++() {
    ++offset_;
    return *this;
  }
  //////////////////////////////////////////////////////////////////////////////
  BitsIterator<T> operator++(int) {
    auto copy = BitsIterator<T>(*this);
    ++offset_;
    return copy;
  }
  //////////////////////////////////////////////////////////////////////////////
  BitsIterator<T>& operator--() {
    --offset_;
    return *this;
  }
  //////////////////////////////////////////////////////////////////////////////
  BitsIterator<T> operator--(int) {
    auto copy = BitsIterator<T>(*this);
    --offset_;
    return copy;
  }
  // NOLINTEND(readability-identifier-naming)

 public:
  //////////////////////////////////////////////////////////////////////////////
  ~BitsIterator() = default;

 private:
  const void* iterated_;
  std::ptrdiff_t offset_;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The ReverseBitsIterator class
///
template <class T>
class ReverseBitsIterator : public std::reverse_iterator<BitsIterator<T>> {
 public:
  constexpr static std::size_t bitsTSize = sizeof(T) * 8;

 public:
  explicit ReverseBitsIterator(const T& iterated,
                               std::ptrdiff_t offset = bitsTSize)
      : std::reverse_iterator<BitsIterator<T>>(
            BitsIterator<T>(iterated, bitsTSize - offset)) {
  }
};

}  // namespace ael::impl

template <class T>
struct std::iterator_traits<ael::impl::BitsIterator<T>> {
  using value_type = bool;
  using reference = void;
  using pointer = void;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;
};

#endif  // AEL_IMPL_BITS_ITERATOR_HPP
