#ifndef ARITHMETIC_CODER_ENCODED_HPP
#define ARITHMETIC_CODER_ENCODED_HPP

#include <boost/range/combine.hpp>
#include <boost/range/irange.hpp>
#include <vector>
#include <array>
#include <cstdint>
#include <cstddef>
#include <ranges>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoderEncoded class
///
class ByteDataConstructor {
public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The ByteBackInserter class
    ///
    class ByteBackInserter;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The BitBackInserter class
    ///
    class BitBackInserter;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The BytesAfterBits class
    ///
    class BytesAfterBits;

public:

    /**
     * @brief ArithmeticCoderEncoded - enpty encoded constructor
     */
    ByteDataConstructor();

    /**
     * @brief putBit - add single bit in the end.
     * @param bit - a bit to set.
     */
    void putBit(bool bit);

    /**
     * @brief putBitsRepeat
     * @param bit - bit which is set
     * @param num - number of bits repeated
     */
    void putBitsRepeat(bool bit, std::size_t num);

    /**
     * @brief putBitsRepeatWithReset
     * @param bit
     * @param num
     */
    void putBitsRepeatWithReset(bool bit, std::size_t& num);

    /**
     * @brief putByte - add byte in the end
     * @param b - byte
     */
    void putByte(std::byte b);

    /**
     * @brief putT - put object as a byte sequence
     * @param s - object
     */
    void putT(auto s);

    /**
     * @brief putTToPosition
     * @param s
     * @param position
     */
    void putTToPosition(auto s, std::size_t position);

    /**
     * @brief data - get data pointer
     * @return data pointer
     */
    template <class T>
    const T* data() const;

    /**
     * @brief size - number of bytes in a file.
     * @return file size.
     */
    std::size_t size() const;

    /**
     * @brief getBitBackInserter returns bit insert iterator to add bits at
     * the end.
     * @return bit insert iterator.
     */
    BitBackInserter getBitBackInserter();

    /**
     * @brief getByteBackInserter returns byte insert iterator to add bytes at
     * the end.
     * @return
     */
    ByteBackInserter getByteBackInserter();

    /**
     * @brief saveSpaceForT
     * @return
     */
    template <class T>
    std::size_t saveSpaceForT();

private:

    template <class T>
    using TBytes = std::array<std::byte, sizeof(T)>;

private:

    void _moveBitFlag();

private:
    std::vector<std::byte> _data;
    std::byte _currBitFlag;
    std::uint8_t _currBitOffset;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteDataConstructor::ByteBackInserter class
///
class ByteDataConstructor::ByteBackInserter {
public:
    using difference_type = std::ptrdiff_t;
public:
    ByteBackInserter(ByteDataConstructor& owner) : _owner(&owner) {};
    ByteBackInserter operator++(int) { return* this; }
    ByteBackInserter& operator++()   { return *this; }
    void operator=(std::byte byte)   { _owner->putByte(byte); }
    ByteBackInserter& operator*()    { return* this; }
private:
    ByteDataConstructor* _owner;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteDataConstructor::BitBackInserter class
///
class ByteDataConstructor::BitBackInserter {
public:
    using difference_type = std::ptrdiff_t;
public:
    BitBackInserter(ByteDataConstructor& owner) : _owner(&owner) {};
    BitBackInserter operator++(int) { return* this; }
    BitBackInserter& operator++()   { return *this; }
    void operator=(bool bit)        { _owner->putBit(bit); }
    BitBackInserter& operator*()    { return *this; }
    ByteBackInserter getBytesIter() { return _owner->getByteBackInserter(); }
private:
    ByteDataConstructor* _owner;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteDataConstructor::BytesAfterBits class
///
class ByteDataConstructor::BytesAfterBits : std::logic_error {
public:
    using std::logic_error::logic_error;
};

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::putT(auto s) {
    auto& bytes = reinterpret_cast<TBytes<decltype(s)>&>(s);
    std::copy(bytes.begin(), bytes.end(), getByteBackInserter());
}

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::putTToPosition(auto s, std::size_t position) {
    if (position + sizeof(s) >= _data.size()) {
        throw std::out_of_range("Can not write to position this count of bytes.");
    }
    auto& bytes = reinterpret_cast<TBytes<decltype(s)>&>(s);
    std::copy(bytes.begin(), bytes.end(), _data.begin() + position);
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
const T* ByteDataConstructor::data() const {
    return reinterpret_cast<const T*>(_data.data());
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
std::size_t ByteDataConstructor::saveSpaceForT() {
    if (_currBitFlag != std::byte{0b10000000}) {
        throw BytesAfterBits("Tried saving bytes with bit flag != 0b10000000.");
    }
    auto ret = _data.size();
    _data.resize(_data.size() + sizeof(T));
    return ret;
}

} // namespace ael

#endif // ARITHMETIC_CODER_ENCODED_HPP
