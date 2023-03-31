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
  ////////////////////////////////////////////////////////////////////////////
  explicit BitsIterator(const T& iterated,
                        std::ptrdiff_t offset = sizeof(T) * 8)
      : iterated_(&iterated), offset_(offset) {
  }
  ////////////////////////////////////////////////////////////////////////////
  BitsIterator& operator=(const BitsIterator& other) = default;

 private:
  ////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::byte getByte_() const {
    return reinterpret_cast<const std::byte*>(iterated_)[getByteIdx_()];
  }
  ////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::size_t getByteIdx_() const {
    return sizeof(T) - offset_ / 8 - 1;
  }
  ////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::uint8_t getInByteIdx_() const {
    return 8 - (offset_ % 8) - 1;
  }

 protected:
  ////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::ptrdiff_t distance_to(const type& rhs) const {
    return rhs.offset_ - offset_;
  }
  ////////////////////////////////////////////////////////////////////////////
  void advance(std::ptrdiff_t n) {
    offset_ += n;
  }
  ////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool dereference() const {
    return ((getByte_() >> getInByteIdx_()) & std::byte{1}) != std::byte{};
  }
  ////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] bool equal(const type& other) const {
    return offset_ == other.offset_;
  }
  ////////////////////////////////////////////////////////////////////////////
  void increment() {
    ++offset_;
  }
  ////////////////////////////////////////////////////////////////////////////
  void decrement() {
    --offset_;
  }

 private:
  const T* iterated_;
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
  explicit ReverseBitsIterator(const T& iterated,
                               std::ptrdiff_t offset = sizeof(T) * 8)
      : std::reverse_iterator<BitsIterator<T>>(
            BitsIterator<T>(iterated, sizeof(T) * 8 - offset)) {
  }
};

}  // namespace ael::impl

#endif  // BITS_ITERATOR_HPP
