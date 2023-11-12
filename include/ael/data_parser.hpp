#ifndef AEL_DATA_PARSER_HPP
#define AEL_DATA_PARSER_HPP

#include <ael/impl/byte_rng_to_bits.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The DataParser class
///
class DataParser {
 public:
  /**
   * @brief DataParser move constructor.
   */
  DataParser(DataParser&&) = default;
  DataParser(const DataParser&) = delete;

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
  [[nodiscard]] std::size_t getNumBytes() const {
    return data_.size();
  }

  /**
   * @brief getNumBits - get number of bits.
   * @return number of bits.
   */
  [[nodiscard]] std::size_t getNumBits() const {
    return bitsView_.size();
  }

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
  [[nodiscard]] auto getBeginBitsIter() const {
    return bitsView_.begin();
  }

  /**
   * @brief getEndBitsIter
   * @return bitsEndIterator
   */
  [[nodiscard]] auto getEndBitsIter() const {
    return bitsView_.end();
  }

  DataParser& operator=(DataParser&& other) = delete;
  DataParser& operator=(const DataParser& other) = delete;

  ~DataParser() = default;

 private:
  void moveInByteOffset_();

 private:
  using BitsView = decltype(impl::to_bits(
      std::declval<const std::span<const std::byte>&>()));

 private:
  const std::span<const std::byte> data_;
  BitsView bitsView_;
  std::span<const std::byte>::iterator dataIter_;
  std::uint8_t inByteOffset_{};

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
  auto* ret_ptr = static_cast<void*>(&ret);
  auto* ret_bytes = static_cast<TBytes*>(ret_ptr);
  for (auto& byte : *ret_bytes) {
    byte = takeByte();
  }
  return ret;
}

}  // namespace ael

#endif  // AEL_DATA_PARSER_HPP
