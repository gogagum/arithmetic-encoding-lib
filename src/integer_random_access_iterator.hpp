#ifndef INTEGER_RANDOM_ACCESS_ITERATOR_HPP
#define INTEGER_RANDOM_ACCESS_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IntegerRandomAccessIterator class
///
template <std::integral T>
class IntegerRandomAccessIterator
        : public boost::iterator_facade<
            IntegerRandomAccessIterator<T>,
            T,
            boost::random_access_traversal_tag,
            T> {
public:
    using type = IntegerRandomAccessIterator<T>;
public:
    //------------------------------------------------------------------------//
    IntegerRandomAccessIterator(T initial = 0) : _val(initial) {}
    //------------------------------------------------------------------------//
    IntegerRandomAccessIterator(const type& other) = default;
protected:
    //------------------------------------------------------------------------//
    std::ptrdiff_t
    distance_to(const type& rhs) const  { return rhs._val - _val; }
    //------------------------------------------------------------------------//
    void advance(std::ptrdiff_t n)      { _val += n; }
    //------------------------------------------------------------------------//
    T dereference() const               { return _val; }
    //------------------------------------------------------------------------//
    bool equal(const type& other) const { return _val == other._val; }
    //------------------------------------------------------------------------//
    void increment()                    { ++_val; }
    //------------------------------------------------------------------------//
    void decrement()                    { --_val; }

public:
    //------------------------------------------------------------------------//
    T operator[](std::ptrdiff_t n) const { return _val + n; }

private:
    T _val;
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


#endif // INTEGER_RANDOM_ACCESS_ITERATOR_HPP
