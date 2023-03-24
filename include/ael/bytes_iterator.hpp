#ifndef BYTES_ITERATOR_HPP
#define BYTES_ITERATOR_HPP

#include <array>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BytesIterator class
///
template <class T>
class BytesIterator : public boost::iterators::iterator_facade<
        BytesIterator<T>,
        std::byte,
        boost::random_access_traversal_tag,
        std::byte> {
private:
    using type = BytesIterator<T>;
public:
    ////////////////////////////////////////////////////////////////////////////
    BytesIterator(const T& iterated, std::ptrdiff_t offset = sizeof(T))
        : _iterated(&iterated), _offset(offset) {}
    ////////////////////////////////////////////////////////////////////////////
    BytesIterator& operator=(const BytesIterator& other) = default;

private:

    ////////////////////////////////////////////////////////////////////////////
    std::size_t _getByteIndex () const  { return sizeof(T) - _offset - 1; }

protected:
    ////////////////////////////////////////////////////////////////////////////
    std::ptrdiff_t
    distance_to(const type& rhs) const  { return rhs._offset - _offset; }
    ////////////////////////////////////////////////////////////////////////////
    void advance(std::ptrdiff_t n)      { _offset += n; }
    ////////////////////////////////////////////////////////////////////////////
    std::byte dereference() const
    { return reinterpret_cast<const std::byte*>(_iterated)[_getByteIndex()]; }
    ////////////////////////////////////////////////////////////////////////////
    bool equal(const type& other) const { return _offset == other._offset; }
    ////////////////////////////////////////////////////////////////////////////
    void increment()                    { ++_offset; }
    ////////////////////////////////////////////////////////////////////////////
    void decrement()                    { --_offset; }

private:
    const T* _iterated;
    std::ptrdiff_t _offset;
private:
    friend class boost::iterators::iterator_core_access;
};

////////////////////////////////////////////////////////////////////////////////
template <class T>
class ReverseBytesIterator : public std::reverse_iterator<BytesIterator<T>> {
public:
    ReverseBytesIterator(const T& iterated, std::ptrdiff_t offset = sizeof(T))
        : std::reverse_iterator<BytesIterator<T>>(
              BytesIterator<T>(iterated, sizeof(T) - offset)) {}
};

}  // namespace ael::impl

#endif // BYTES_ITERATOR_HPP
