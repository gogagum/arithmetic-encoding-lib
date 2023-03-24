#include <boost/range/irange.hpp>
#include <ael/data_parser.hpp>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
DataParser::DataParser(std::span<const std::byte> data)
    : _data(data),
      _dataIter{_data.begin()},
      _inByteOffset(0) { }

////////////////////////////////////////////////////////////////////////////////
std::byte DataParser::takeByte() {
    if (_inByteOffset == 0) {
        std::byte ret = *_dataIter;
        ++_dataIter;
        return ret;
    }
    auto ret = std::byte{0};
    for ([[maybe_unused]] std::size_t _: boost::irange(0, 8)) {
        ret <<= 1;
        ret |= takeBit() ? std::byte{0b00000001} : std::byte{0b00000000};
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
bool DataParser::takeBit() {
    if (_dataIter >= _data.end()) {
        return false;
    }
    bool ret = (*_dataIter & _getByteFlag()) != std::byte{0};
    _moveInByteOffset();
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
void DataParser::_moveInByteOffset() {
    ++_inByteOffset;
    if (_inByteOffset == 8) {
        _inByteOffset = 0;
        ++_dataIter;
    }
}

////////////////////////////////////////////////////////////////////////////////
DataParser& DataParser::seek(std::size_t bitsOffset) {
    _dataIter = _data.begin() + bitsOffset / 8;
    _inByteOffset = bitsOffset % 8;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
bool operator==(const DataParser& dp1, const DataParser& dp2) {
    return dp1._data.data() == dp2._data.data()
            && dp1._data.size() == dp2._data.size()
            && dp1._dataIter == dp2._dataIter;
}

}  // namespace ael
