#include <boost/range/irange.hpp>

#include <ael/byte_data_constructor.hpp>
#include <ael/bits_iterator.hpp>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
ByteDataConstructor::ByteDataConstructor() : _currBitFlag{0b10000000} {}

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::putBit(bool bit) {
    if (_currBitFlag == std::byte{0b10000000}) {
        _data.push_back(std::byte{0b00000000});
    }
    if (bit) {
        *_data.rbegin() |= _currBitFlag;
    } else {
        *_data.rbegin() &= ~_currBitFlag;
    }
    _moveBitFlag();
}

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::putBitsRepeat(bool bit, std::size_t num) {
    std::fill_n(getBitBackInserter(), num, bit);
}

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::putBitsRepeatWithReset(bool bit, std::size_t& num) {
    putBitsRepeat(bit, num);
    num = 0;
}

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::putByte(std::byte b) {
    if (_currBitFlag == std::byte{0b10000000}) {
        _data.push_back(b);
    } else {
        std::copy_n(impl::BitsIterator(b, 0), 8, getBitBackInserter());
    }
}

////////////////////////////////////////////////////////////////////////////////
std::size_t ByteDataConstructor::size() const {
    return _data.size();
}

////////////////////////////////////////////////////////////////////////////////
auto ByteDataConstructor::getBitBackInserter() -> BitBackInserter {
    return BitBackInserter(*this);
}

////////////////////////////////////////////////////////////////////////////////
auto ByteDataConstructor::getByteBackInserter() -> ByteBackInserter {
    return ByteBackInserter(*this);
}

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::_moveBitFlag() {
    _currBitFlag >>= 1;
    if (_currBitFlag == std::byte{0b00000000}) {
        _currBitFlag = std::byte{0b10000000};
    }
}

}  // namespace ael
