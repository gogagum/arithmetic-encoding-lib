#ifndef BITS_ITERATOR_HPP
#define BITS_ITERATOR_HPP

#include <array>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsIterator class
///
template <class T>
class BitsIterator : public boost::iterators::iterator_facade<
        BitsIterator<T>,
        bool,
        boost::random_access_traversal_tag,
        bool> {
private:
    using type = BitsIterator<T>;
public:

    ////////////////////////////////////////////////////////////////////////////
    BitsIterator(const T& iterated, std::ptrdiff_t offset = sizeof(T) * 8)
        : _iterated(&iterated), _offset(offset) {}
    ////////////////////////////////////////////////////////////////////////////
    BitsIterator& operator=(const BitsIterator& other) = default;

private:
    ////////////////////////////////////////////////////////////////////////////
    std::byte _getByte() const
    { return reinterpret_cast<const std::byte*>(_iterated)[_getByteIdx()]; }
    ////////////////////////////////////////////////////////////////////////////
    std::size_t _getByteIdx() const      { return sizeof(T) - _offset / 8 - 1; }
    ////////////////////////////////////////////////////////////////////////////
    std::uint8_t _getInByteIdx() const   { return 8 - (_offset % 8) - 1; }
protected:
    ////////////////////////////////////////////////////////////////////////////
    std::ptrdiff_t
    distance_to(const type& rhs) const   { return rhs._offset - _offset; }
    ////////////////////////////////////////////////////////////////////////////
    void advance(std::ptrdiff_t n)       { _offset += n; }
    ////////////////////////////////////////////////////////////////////////////
    bool dereference() const
    { return ((_getByte() >> _getInByteIdx()) & std::byte{1}) != std::byte{}; }
    ////////////////////////////////////////////////////////////////////////////
    bool equal(const type& other) const  { return _offset == other._offset; }
    ////////////////////////////////////////////////////////////////////////////
    void increment()                     { ++_offset; }
    ////////////////////////////////////////////////////////////////////////////
    void decrement()                     { --_offset; }

private:
    const T* _iterated;
    std::ptrdiff_t _offset;
private:
    friend class boost::iterators::iterator_core_access;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The ReverseBitsIterator class
///
template <class T>
class ReverseBitsIterator : public std::reverse_iterator<BitsIterator<T>> {
public:
    ReverseBitsIterator(const T& iterated,
                        std::ptrdiff_t offset = sizeof(T) * 8)
        : std::reverse_iterator<BitsIterator<T>>(
              BitsIterator<T>(iterated, sizeof(T) * 8 - offset)) {}
};

}  // namespace ael::impl

#endif // BITS_ITERATOR_HPP
