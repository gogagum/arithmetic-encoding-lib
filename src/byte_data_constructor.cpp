#include <ael/bits_iterator.hpp>
#include <ael/byte_data_constructor.hpp>
#include <boost/range/irange.hpp>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
ByteDataConstructor::ByteDataConstructor() : currBitFlag_{0b10000000} {
}

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::putBit(bool bit) {
  if (std::byte{0b10000000} == currBitFlag_) {
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
void ByteDataConstructor::putByte(std::byte b) {
  if (currBitFlag_ == std::byte{0b10000000}) {
    data_.push_back(b);
  } else {
    std::copy_n(impl::BitsIterator(b, 0), 8, getBitBackInserter());
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
    currBitFlag_ = std::byte{0b10000000};
  }
}

}  // namespace ael
