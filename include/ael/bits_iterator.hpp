#ifndef BITS_ITERATOR_HPP
#define BITS_ITERATOR_HPP

#include <array>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <cstddef>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsIterator class
///
template <class T>
class BitsIterator
    : public boost::iterators::iterator_facade<
          BitsIterator<T>, bool, boost::random_access_traversal_tag, bool> {
 private:
  using type = BitsIterator<T>;

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

 protected:
  // NOLINTBEGIN(readability-identifier-naming)
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::ptrdiff_t distance_to(const type& rhs) const {
    return rhs.offset_ - offset_;
  }
  //////////////////////////////////////////////////////////////////////////////
  void advance(std::ptrdiff_t n) {
    offset_ += n;
  }
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool dereference() const {
    return ((getByte_() >> getInByteIdx_()) & std::byte{1}) != std::byte{};
  }
  //////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool equal(const type& other) const {
    return offset_ == other.offset_;
  }
  //////////////////////////////////////////////////////////////////////////////
  void increment() {
    ++offset_;
  }
  //////////////////////////////////////////////////////////////////////////////
  void decrement() {
    --offset_;
  }
  // NOLINTEND(readability-identifier-naming)

 public:
  //////////////////////////////////////////////////////////////////////////////
  ~BitsIterator() = default;

 private:
  const void* iterated_;
  std::ptrdiff_t offset_;

 private:
  friend class boost::iterators::iterator_core_access;
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

#endif  // BITS_ITERATOR_HPP
