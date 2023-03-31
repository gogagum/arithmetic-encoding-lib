#include <ael/data_parser.hpp>
#include <boost/range/irange.hpp>
#include <cstddef>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
DataParser::DataParser(std::span<const std::byte> data)
    : data_(data), dataIter_{data_.begin()}, inByteOffset_(0) {
}

////////////////////////////////////////////////////////////////////////////////
std::byte DataParser::takeByte() {
  if (inByteOffset_ == 0) {
    const std::byte ret = *dataIter_;
    ++dataIter_;
    return ret;
  }
  auto ret = std::byte{0};
  for ([[maybe_unused]] const std::size_t _ :
       boost::irange<std::size_t>(0, 8)) {
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
  const bool ret = (*dataIter_ & getByteFlag_()) != std::byte{0};
  moveInByteOffset_();
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
void DataParser::moveInByteOffset_() {
  ++inByteOffset_;
  if (inByteOffset_ == 8) {
    inByteOffset_ = 0;
    ++dataIter_;
  }
}

////////////////////////////////////////////////////////////////////////////////
DataParser& DataParser::seek(std::size_t bitsOffset) {
  dataIter_ = data_.begin() + static_cast<ptrdiff_t>(bitsOffset / 8);
  inByteOffset_ = bitsOffset % 8;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
bool operator==(const DataParser& dp1, const DataParser& dp2) {
  return dp1.data_.data() == dp2.data_.data() &&
         dp1.data_.size() == dp2.data_.size() && dp1.dataIter_ == dp2.dataIter_;
}

}  // namespace ael
