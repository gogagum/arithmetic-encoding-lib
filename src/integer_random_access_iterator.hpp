#ifndef INTEGER_RANDOM_ACCESS_ITERATOR_HPP
#define INTEGER_RANDOM_ACCESS_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IntegerRandomAccessIterator class
///
template <std::integral T>
class IntegerRandomAccessIterator
    : public boost::iterator_facade<IntegerRandomAccessIterator<T>, T,
                                    boost::random_access_traversal_tag, T> {
 public:
  using type = IntegerRandomAccessIterator<T>;

 public:
  //--------------------------------------------------------------------------//
  explicit IntegerRandomAccessIterator(T initial = 0) : val_(initial) {
  }
  //--------------------------------------------------------------------------//
  IntegerRandomAccessIterator(const type& other) = default;

 protected:
  //--------------------------------------------------------------------------//
  [[nodiscard]] std::ptrdiff_t distance_to(const type& rhs) const {
    return rhs.val_ - val_;
  }
  //--------------------------------------------------------------------------//
  void advance(std::ptrdiff_t n) {
    val_ += n;
  }
  //--------------------------------------------------------------------------//
  [[nodiscard]] T dereference() const {
    return val_;
  }
  //--------------------------------------------------------------------------//
  [[nodiscard]] bool equal(const type& other) const {
    return val_ == other.val_;
  }
  //--------------------------------------------------------------------------//
  void increment() {
    ++val_;
  }
  //--------------------------------------------------------------------------//
  void decrement() {
    --val_;
  }

 public:
  //--------------------------------------------------------------------------//
  T operator[](std::ptrdiff_t n) const {
    return val_ + n;
  }

 private:
  T val_;

 private:
  friend class boost::iterators::iterator_core_access;
};

}  // namespace ael::impl

template <std::integral T>
struct std::iterator_traits<ael::impl::IntegerRandomAccessIterator<T>> {
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using reference = T;
};

#endif  // INTEGER_RANDOM_ACCESS_ITERATOR_HPP
