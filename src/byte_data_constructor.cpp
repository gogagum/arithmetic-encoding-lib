#include <ael/byte_data_constructor.hpp>
#include <algorithm>
#include <climits>
#include <ranges>

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
    auto bits = std::views::iota(std::size_t{0}, std::size_t{CHAR_BIT}) |
                std::views::reverse |
                std::views::transform([byteToPut](std::size_t i) {
                  return ((byteToPut >> i) & std::byte{1}) == std::byte{1};
                });

    std::ranges::copy(bits, getBitBackInserter());
  }
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
