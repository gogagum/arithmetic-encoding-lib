#ifndef AEL_IMPL_DATA_PARSER_HPP
#define AEL_IMPL_DATA_PARSER_HPP

#include <array>
#include <cstddef>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>
#include <cstdint>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoderDecoded class
///
class DataParser {
 public:
  ////////////////////////////////////////////////////////////////////////////
  /// \brief The BitIterator class
  class BitsIterator {
   public:
    BitsIterator(DataParser& owner, std::size_t bitsPosition)
        : owner_(&owner), bitsPosition_(bitsPosition) {
    }

    BitsIterator(const BitsIterator& other) = default;

    BitsIterator(BitsIterator&& other) noexcept = default;

    // NOLINTBEGIN(readability-identifier-naming)
    ////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] bool operator*() const {
      return owner_->seek(bitsPosition_).takeBit();
    }
    ////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] bool operator==(const BitsIterator& other) const {
      return bitsPosition_ == other.bitsPosition_;
    }
    ////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] std::ptrdiff_t operator-(const BitsIterator& other) const {
      return static_cast<ptrdiff_t>(other.bitsPosition_) -
             static_cast<ptrdiff_t>(bitsPosition_);
    }
    ////////////////////////////////////////////////////////////////////////////
    BitsIterator& operator=(const BitsIterator& other) = default;
    ////////////////////////////////////////////////////////////////////////////
    BitsIterator& operator=(BitsIterator&& other) = default;
    ////////////////////////////////////////////////////////////////////////////
    BitsIterator& operator++() {
      ++bitsPosition_;
      return *this;
    }
    ////////////////////////////////////////////////////////////////////////////
    BitsIterator operator++(int) {
      auto retCopy = *this;
      ++bitsPosition_;
      return retCopy;
    }
    ////////////////////////////////////////////////////////////////////////////
    void operator+=(std::ptrdiff_t offset) {
      bitsPosition_ += offset;
    }
    ////////////////////////////////////////////////////////////////////////////
    ~BitsIterator() = default;

    // NOLINTEND(readability-identifier-naming)

   private:
    DataParser* owner_;
    std::size_t bitsPosition_;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The OutOfRange class
  class OutOfRange;

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
    constexpr const auto bitsInBytes = 8;
    return data_.size() * bitsInBytes;
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
  auto getBeginBitsIter() {
    return BitsIterator(*this, 0);
  }

  /**
   * @brief getEndBitsIter
   * @return bitsEndIterator
   */
  auto getEndBitsIter() {
    constexpr const auto bitsInBytes = 8;
    return BitsIterator(*this, data_.size() * bitsInBytes);
  }

  DataParser& operator=(DataParser&& other) = delete;
  DataParser& operator=(const DataParser& other) = delete;

  ~DataParser() = default;

 private:
  void moveInByteOffset_();

  [[nodiscard]] std::byte getByteFlag_() const {
    constexpr const auto startByte = std::byte{0b10000000};
    return startByte >> inByteOffset_;
  }

 private:
  const std::span<const std::byte> data_;
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
  auto* retPtr = static_cast<void*>(&ret);
  auto* retBytes = static_cast<TBytes*>(retPtr);
  for (auto& byte : *retBytes) {
    byte = takeByte();
  }
  return ret;
}

}  // namespace ael

template <>
struct std::iterator_traits<ael::DataParser::BitsIterator> {
  using difference_type = std::ptrdiff_t;
  using value_type = bool;
  using pointer = void;
  using reference = void;
  using iterator_category = std::input_iterator_tag;
};

#endif  // AEL_IMPL_DATA_PARSER_HPP
