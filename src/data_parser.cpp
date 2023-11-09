#include <ael/data_parser.hpp>
#include <ael/impl/byte_rng_to_bits.hpp>
#include <climits>
#include <cstddef>
#include <ranges>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
DataParser::DataParser(std::span<const std::byte> data)
    : data_(data), bitsView_{impl::to_bits(data_)}, dataIter_{data_.begin()} {
}

////////////////////////////////////////////////////////////////////////////////
std::byte DataParser::takeByte() {
  if (inByteOffset_ == 0) {
    const std::byte ret = *dataIter_;
    ++dataIter_;
    return ret;
  }
  auto ret = std::byte{0};
  for ([[maybe_unused]] const auto _ : std::ranges::iota_view(0, 8)) {
    ret <<= 1;
    ret |= takeBit() ? std::byte{0b00000001} : std::byte{0b00000000};
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
bool DataParser::takeBit() {
  if (dataIter_ >= data_.end()) {
    return false;
  }
  const bool ret =
      bitsView_[(dataIter_ - data_.begin()) * CHAR_BIT + inByteOffset_];
  moveInByteOffset_();
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
void DataParser::moveInByteOffset_() {
  ++inByteOffset_;
  if (inByteOffset_ == CHAR_BIT) {
    inByteOffset_ = 0;
    ++dataIter_;
  }
}

////////////////////////////////////////////////////////////////////////////////
DataParser& DataParser::seek(std::size_t bitsOffset) {
  dataIter_ = data_.begin() + static_cast<ptrdiff_t>(bitsOffset / CHAR_BIT);
  inByteOffset_ = bitsOffset % CHAR_BIT;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
bool operator==(const DataParser& dp1, const DataParser& dp2) {
  return dp1.data_.data() == dp2.data_.data() &&
         dp1.data_.size() == dp2.data_.size() && dp1.dataIter_ == dp2.dataIter_;
}

}  // namespace ael
