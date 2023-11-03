#ifndef AEL_ARITHMETIC_CODER_HPP
#define AEL_ARITHMETIC_CODER_HPP

#include <ael/byte_data_constructor.hpp>
#include <ael/impl/ranges_calc.hpp>
#include <boost/container/static_vector.hpp>
#include <cstdint>
#include <memory>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
class ArithmeticCoder {
 public:
  struct EncodeRet {
    std::unique_ptr<ByteDataConstructor> encoded;
    std::size_t wordsCount;
    std::size_t bitsEncoded;
  };

 public:
  explicit ArithmeticCoder(
      std::unique_ptr<ByteDataConstructor>&& dataConstructor =
          std::make_unique<ByteDataConstructor>());

  /**
   * @brief encode - encode byte flow.
   * @param ordFlow orders range.
   * @param dataConstructor encoded sequence constructor.
   * @param dict dictionary with words statistics.
   * @return [wordsCount, bitsEncoded]
   */
  template <class DictT>
  ArithmeticCoder&& encode(auto ordFlow, DictT& dict);

  /**
   * @brief encode - encode byte flow.
   * @param ordFlow orders range.
   * @param dataConstructor encoded sequence constructor.
   * @param dict dictionary with words statistics.
   * @param tick function, which will be called every time when one element is
   * encoded.
   * @return [wordsCount, bitsEncoded]
   */
  template <class DictT>
  ArithmeticCoder&& encode(
      auto ordFlow, DictT& dict, auto tick = [] {});

  EncodeRet finalize() &&;

 private:
  [[nodiscard]] ByteDataConstructor& getEncoded_() const;

 private:
  EncodeRet ret_;
  std::size_t btf_{0};
  bool finalizeChoice_{false};
};

////////////////////////////////////////////////////////////////////////////////
inline ArithmeticCoder::ArithmeticCoder(
    std::unique_ptr<ByteDataConstructor>&& dataConstructor)
    : ret_{std::move(dataConstructor), 0, 0} {
}

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
ArithmeticCoder&& ArithmeticCoder::encode(auto ordFlow, DictT& dict) {
  return encode(ordFlow, dict, [] {});
}

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
ArithmeticCoder&& ArithmeticCoder::encode(auto ordFlow, DictT& dict,
                                          auto tick) {
  using RC = impl::RangesCalc<typename DictT::Count, DictT::countNumBits>;
  auto currRange = typename RC::Range{0, RC::total};

  for (auto ord : ordFlow) {
    const auto [low, high, total] = dict.getProbabilityStats(ord);
    currRange = RC::rangeFromStatsAndPrev(currRange, {low, high, total});

    while (true) {
      if (currRange.high <= RC::half) {
        ret_.bitsEncoded += btf_ + 1;
        getEncoded_().putBit(false);
        getEncoded_().putBitsRepeatWithReset(true, btf_);
      } else if (currRange.low >= RC::half) {
        ret_.bitsEncoded += btf_ + 1;
        getEncoded_().putBit(true);
        getEncoded_().putBitsRepeatWithReset(false, btf_);
      } else if (currRange.low >= RC::quarter &&
                 currRange.high <= RC::threeQuarters) {
        ++btf_;
      } else {
        break;
      }
      currRange = RC::recalcRange(currRange);
    }
    ++ret_.wordsCount;
    tick();
  }

  finalizeChoice_ = currRange.low < RC::quarter;
  return std::move(*this);
}

////////////////////////////////////////////////////////////////////////////////
inline ByteDataConstructor& ArithmeticCoder::getEncoded_() const {
  return *ret_.encoded;
}

}  // namespace ael

#endif  // AEL_ARITHMETIC_CODER_HPP
