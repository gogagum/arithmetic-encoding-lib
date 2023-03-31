#ifndef BYTES_ITERATOR_HPP
#define BYTES_ITERATOR_HPP

#include <array>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <cstddef>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BytesIterator class
///
template <class T>
class BytesIterator : public boost::iterators::iterator_facade<
                          BytesIterator<T>, std::byte,
                          boost::random_access_traversal_tag, std::byte> {
 private:
  using type = BytesIterator<T>;

 public:
  ////////////////////////////////////////////////////////////////////////////
  explicit BytesIterator(const T& iterated, std::ptrdiff_t offset = sizeof(T))
      : iterated_(&iterated), offset_(offset) {
  }
  ////////////////////////////////////////////////////////////////////////////
  BytesIterator& operator=(const BytesIterator& other) = default;

 private:
  ////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::size_t getByteIndex_() const {
    return sizeof(T) - offset_ - 1;
  }

 protected:
  ////////////////////////////////////////////////////////////////////////////
  std::ptrdiff_t distance_to(const type& rhs) const {
    return rhs.offset_ - offset_;
  }
  ////////////////////////////////////////////////////////////////////////////
  void advance(std::ptrdiff_t n) {
    offset_ += n;
  }
  ////////////////////////////////////////////////////////////////////////////
  [[nodiscard]] std::byte dereference() const {
    return reinterpret_cast<const std::byte*>(iterated_)[getByteIndex_()];
  }
  ////////////////////////////////////////////////////////////////////////////
  bool equal(const type& other) const {
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

#endif  // BYTES_ITERATOR_HPP
