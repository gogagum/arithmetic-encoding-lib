#ifndef AEL_BYTE_DATA_CONSTRUCTOR_HPP
#define AEL_BYTE_DATA_CONSTRUCTOR_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <vector>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoderEncoded class
///
class ByteDataConstructor {
 private:
  constexpr static auto startMask_ = std::byte{0b10000000};

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
  ByteDataConstructor() = default;

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
   * @param byteToPut - byte
   */
  void putByte(std::byte byteToPut);

  /**
   * @brief putT - put object as a byte sequence
   * @param element - object
   */
  void putT(auto element);

  /**
   * @brief putTToPosition
   * @param element
   * @param position
   */
  void putTToPosition(auto element, std::size_t position);

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
  [[nodiscard]] std::size_t size() const {
    return data_.size();
  }

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
  void moveBitFlag_();

 private:
  std::vector<std::byte> data_{};
  std::byte currBitFlag_{startMask_};
  std::uint8_t currBitOffset_{0};
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteDataConstructor::ByteBackInserter class
///
class ByteDataConstructor::ByteBackInserter {
 public:
  using difference_type = std::ptrdiff_t;

 public:
  explicit ByteBackInserter(ByteDataConstructor& owner) : owner_(&owner){};
  ByteBackInserter operator++(int) {
    return *this;
  }
  ByteBackInserter& operator++() {
    return *this;
  }
  ByteBackInserter& operator=(std::byte byte) {
    owner_->putByte(byte);
    return *this;
  }
  ByteBackInserter& operator*() {
    return *this;
  }

 private:
  ByteDataConstructor* owner_;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteDataConstructor::BitBackInserter class
///
class ByteDataConstructor::BitBackInserter {
 public:
  using difference_type = std::ptrdiff_t;

 public:
  explicit BitBackInserter(ByteDataConstructor& owner) : owner_(&owner){};
  BitBackInserter operator++(int) {
    return *this;
  }
  BitBackInserter& operator++() {
    return *this;
  }
  BitBackInserter& operator=(bool bit) {
    owner_->putBit(bit);
    return *this;
  }
  BitBackInserter& operator*() {
    return *this;
  }
  ByteBackInserter getBytesIter() {
    return owner_->getByteBackInserter();
  }

 private:
  ByteDataConstructor* owner_;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteDataConstructor::BytesAfterBits class
///
class ByteDataConstructor::BytesAfterBits : std::logic_error {
 public:
  using std::logic_error::logic_error;
};

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::putT(auto element) {
  auto* elPtr = static_cast<void*>(&element);
  auto* bytes = static_cast<TBytes<decltype(element)>*>(elPtr);
  std::copy(bytes->begin(), bytes->end(), getByteBackInserter());
}

////////////////////////////////////////////////////////////////////////////////
void ByteDataConstructor::putTToPosition(auto element, std::size_t position) {
  if (position + sizeof(element) >= data_.size()) {
    throw std::out_of_range("Can not write to position this count of bytes.");
  }
  auto* elPtr = static_cast<void*>(&element);
  auto* bytes = static_cast<TBytes<decltype(element)>*>(elPtr);
  std::copy(bytes->begin(), bytes->end(),
            data_.begin() + static_cast<std::ptrdiff_t>(position));
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
const T* ByteDataConstructor::data() const {
  return data_.data();
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
std::size_t ByteDataConstructor::saveSpaceForT() {
  if (currBitFlag_ != startMask_) {
    throw BytesAfterBits("Tried saving bytes with bit flag != 0b10000000.");
  }
  const auto ret = data_.size();
  data_.resize(data_.size() + sizeof(T));
  return ret;
}

}  // namespace ael

#endif  // AEL_BYTE_DATA_CONSTRUCTOR_HPP
