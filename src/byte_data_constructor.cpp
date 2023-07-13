#include <ael/bits_iterator.hpp>
#include <ael/byte_data_constructor.hpp>
#include <algorithm>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::putBit(bool bit) {
  if (startMask_ == currBitFlag_) {
    data_.push_back(std::byte{0b00000000});
  }
  if (bit) {
    *data_.rbegin() |= currBitFlag_;
  } else {
    *data_.rbegin() &= ~currBitFlag_;
  }
  moveBitFlag_();
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
void ByteDataConstructor::putByte(std::byte byteToPut) {
  if (currBitFlag_ == startMask_) {
    data_.push_back(byteToPut);
  } else {
    constexpr std::size_t bitsInByte = 8;
    std::copy_n(impl::BitsIterator(byteToPut, 0), bitsInByte,
                getBitBackInserter());
  }
}

////////////////////////////////////////////////////////////////////////////////
std::size_t ByteDataConstructor::size() const {
  return data_.size();
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
void ByteDataConstructor::moveBitFlag_() {
  currBitFlag_ >>= 1;
  if (std::byte{0b00000000} == currBitFlag_) {
    currBitFlag_ = startMask_;
  }
}

}  // namespace ael
