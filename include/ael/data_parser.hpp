#pragma once

#ifndef ARITHMETIC_DECODER_DECODED_HPP
#define ARITHMETIC_DECODER_DECODED_HPP

#include <array>
#include <cstddef>
#include <ranges>
#include <span>
#include <vector>
#include <stdexcept>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/iterator_range.hpp>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoderDecoded class
///
class DataParser {
public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The BitIterator class
    class BitsIterator: public boost::iterators::iterator_facade<
            BitsIterator,
            bool,
            boost::random_access_traversal_tag,
            bool
        > {
    public:
        using type = BitsIterator;
    public:
        BitsIterator(DataParser& owner, std::size_t bitsPosition)
            : _owner(&owner), _bitsPosition(bitsPosition) {}
    protected:
        ////////////////////////////////////////////////////////////////////////
        bool dereference() const
        { return _owner->seek(_bitsPosition).takeBit(); }
        ////////////////////////////////////////////////////////////////////////
        bool equal(const type& other) const
        { return _bitsPosition == other._bitsPosition; }
        ////////////////////////////////////////////////////////////////////////
        std::ptrdiff_t distance_to(const type& other) const
        { return static_cast<ptrdiff_t>(other._bitsPosition) - _bitsPosition; }
        ////////////////////////////////////////////////////////////////////////
        void increment()                     { ++_bitsPosition; }
        ////////////////////////////////////////////////////////////////////////
        void advance(std::ptrdiff_t offset)  { _bitsPosition += offset; }
    private:
        DataParser* _owner;
        std::size_t _bitsPosition;
    private:
        friend class boost::iterators::iterator_core_access;
    };

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The OutOfRange class
    class OutOfRange;

public:

    /**
     * @brief DataParser move constructor. 
     */
    DataParser(DataParser&&) = default;

    /**
     * @brief ArithmeticDecoderDecoded
     * @param data
     */
    explicit DataParser(std::span<const std::byte> data);

    /**
     * @brief takeByte take one byte.
     * @return one byte.
     */
    std::byte takeByte();

    /**
     * @brief takeBit get one bit from current position and move by one bit.
     * @return one bit.
     */
    bool takeBit();

    /**
     * @brief takeT get T as sizeof(T) bits.
     * @return object of type T.
     */
    template <class T>
    T takeT();

    /**
     * @brief getNumBytes - get number of bytes parsed.
     * @return number of bytes.
     */
    std::size_t getNumBytes() const { return _data.size(); }

    /**
     * @brief getNumBits - get number of bits.
     * @return number of bits.
     */
    std::size_t getNumBits() const { return _data.size() * 8; }

    /**
     * @brief seek move to bitsOffset position.
     * @param bitsOffset - position to move to.
     * @return reference to self.
     */
    DataParser& seek(std::size_t bitsOffset);

    /**
     * @brief getBeginBitsIter
     * @return bits begin iterator.
     */
    auto getBeginBitsIter() { return BitsIterator(*this, 0); }

    /**
     * @brief getEndBitsIter
     * @return bitsEndIterator
     */
    auto getEndBitsIter() { return BitsIterator(*this, _data.size() * 8); }

private:

    void _moveInByteOffset();

    std::byte
    _getByteFlag() const { return std::byte{0b10000000} >> _inByteOffset; }

private:

    const std::span<const std::byte> _data;
    std::span<const std::byte>::iterator _dataIter;
    std::uint8_t _inByteOffset;

private:

    friend bool operator==(const DataParser& dp1, const DataParser& dp2);
};

////////////////////////////////////////////////////////////////////////////////
bool operator==(const DataParser& dp1, const DataParser& dp2);

////////////////////////////////////////////////////////////////////////////////
template <class T>
T DataParser::takeT() {
    using TBytes = std::array<std::byte, sizeof(T)>;

    auto ret = T();
    auto& bytes = reinterpret_cast<TBytes&>(ret);
    for (auto& byte: bytes) {
        byte = takeByte();
    }
    return ret;
}


}  // namespace ael

#endif  // ARITHMETIC_DECODER_DECODED_HPP
