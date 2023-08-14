#ifndef AEL_IMPL_BYTES_ITERATOR_HPP
#define AEL_IMPL_BYTES_ITERATOR_HPP

#include <array>
#include <cstddef>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BytesIterator class
///
template <class T>
class BytesIterator {
 private:
  constexpr const static auto size_ = sizeof(T);

 public:
  //////////////////////////////////////////////////////////////////////////////
  explicit BytesIterator(const T& iterated, std::ptrdiff_t offset = sizeof(T))
      : iterated_(&iterated), offset_(offset) {
  }
  //////////////////////////////////////////////////////////////////////////////
  BytesIterator(const BytesIterator&) = default;
  //////////////////////////////////////////////////////////////////////////////
  BytesIterator(BytesIterator&&) noexcept = default;
  //////////////////////////////////////////////////////////////////////////////
  BytesIterator& operator=(const BytesIterator& other) = default;
  //////////////////////////////////////////////////////////////////////////////
  BytesIterator& operator=(BytesIterator&& other) noexcept = default;

 private:
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::size_t getByteIndex_() const {
    return sizeof(T) - offset_ - 1;
  }

 public:
  //NOLINTBEGIN(readability-identifier-naming)
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::ptrdiff_t operator-(const BytesIterator& rhs) const {
    return rhs.offset_ - offset_;
  }
  //////////////////////////////////////////////////////////////////////////////
  void operator+=(std::ptrdiff_t n) {
    offset_ += n;
  }
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::byte operator*() const {
    const auto& bytes = *static_cast<const std::array<const std::byte, size_>*>(
        iterated_);
    return bytes[getByteIndex_()];//NOLINT
  }
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool operator==(const BytesIterator<T>& other) const {
    return offset_ == other.offset_;
  }
  //////////////////////////////////////////////////////////////////////////////
  BytesIterator& operator++() {
    ++offset_;
    return *this;
  }
  //////////////////////////////////////////////////////////////////////////////
  BytesIterator operator++(int) {
    auto copy = BytesIterator(*this);
    ++offset_;
    return copy;
  }
  //////////////////////////////////////////////////////////////////////////////
  BytesIterator& operator--() {
    --offset_;
    return *this;
  }
  //////////////////////////////////////////////////////////////////////////////
  BytesIterator operator--(int) {
    auto copy = BytesIterator(*this);
    --offset_;
    return copy;
  }
  // NOLINTEND(readability-identifier-naming)
 public:
  //////////////////////////////////////////////////////////////////////////////
  ~BytesIterator() = default;

 private:
  const void* iterated_;
  std::ptrdiff_t offset_;
};

////////////////////////////////////////////////////////////////////////////////
template <class T>
class ReverseBytesIterator : public std::reverse_iterator<BytesIterator<T>> {
 public:
  explicit ReverseBytesIterator(const T& iterated,
                                std::ptrdiff_t offset = sizeof(T))
      : std::reverse_iterator<BytesIterator<T>>(
            BytesIterator<T>(iterated, sizeof(T) - offset)) {
  }
};

}  // namespace ael::impl

template<class T>
struct std::iterator_traits<ael::impl::BytesIterator<T>> {
  using value_type = std::byte;
  using pointer = void;
  using reference = void;
  using iterator_category = std::input_iterator_tag;
  using difference_type = std::ptrdiff_t;
};

#endif  // AEL_IMPL_BYTES_ITERATOR_HPP
