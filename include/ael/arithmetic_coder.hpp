#ifndef AEL_ARITHMETIC_CODER_HPP
#define AEL_ARITHMETIC_CODER_HPP

#include <ael/byte_data_constructor.hpp>
#include <ael/impl/ranges_calc.hpp>
#include <memory>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
class ArithmeticCoder {
 public:
  struct Stats {
    std::size_t wordsCount;
    std::size_t bitsEncoded;
  };

  struct FinalRet {
    std::unique_ptr<ByteDataConstructor> dataConstructor;
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

  /**
   * @brief Get encoded orders and encoded bits counts change since last
   * `getStatsChange()` call. On a first call returns just encoded orders and
   * encoded bits counts.
   *
   * @return [encodedOrdersChange, encodedBitsChange]
   */
  Stats getStatsChange();

  FinalRet finalize() &&;

 private:
  template <class RC>
  RC::Range calcRange_();

 private:
  struct TmpRange_ {
    using WideNum = boost::multiprecision::uint256_t;
    WideNum low;
    WideNum high;
    WideNum total;
  };

 private:
  std::unique_ptr<ByteDataConstructor> dataConstructor_;
  std::size_t bitsEncoded_{0};
  std::size_t wordsCnt_{0};
  std::size_t prevBitsEncoded_{0};
  std::size_t prevWordsCnt_{0};
  std::size_t btf_{0};
  bool finalizeChoice_{false};
  TmpRange_ prevRange_{0, 1, 1};
};

////////////////////////////////////////////////////////////////////////////////
inline ArithmeticCoder::ArithmeticCoder(
    std::unique_ptr<ByteDataConstructor>&& dataConstructor)
    : dataConstructor_{std::move(dataConstructor)} {
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
  auto currRange = calcRange_<RC>();

  for (auto ord : ordFlow) {
    const auto [low, high, total] = dict.getProbabilityStats(ord);
    currRange = RC::rangeFromStatsAndPrev(currRange, {low, high, total});

    while (true) {
      if (currRange.high <= RC::half) {
        bitsEncoded_ += btf_ + 1;
        dataConstructor_->putBit(false);
        dataConstructor_->putBitsRepeatWithReset(true, btf_);
      } else if (currRange.low >= RC::half) {
        bitsEncoded_ += btf_ + 1;
        dataConstructor_->putBit(true);
        dataConstructor_->putBitsRepeatWithReset(false, btf_);
      } else if (currRange.low >= RC::quarter &&
                 currRange.high <= RC::threeQuarters) {
        ++btf_;
      } else {
        break;
      }
      currRange = RC::recalcRange(currRange);
    }
    ++wordsCnt_;
    tick();
  }

  finalizeChoice_ = currRange.low < RC::quarter;
  prevRange_.low = currRange.low;
  prevRange_.high = currRange.high;
  prevRange_.total = RC::total;

  return std::move(*this);
}

////////////////////////////////////////////////////////////////////////////////
template <class RC>
auto ArithmeticCoder::calcRange_() -> RC::Range {
  auto retLow = impl::multiply_and_divide(
      prevRange_.low, TmpRange_::WideNum{RC::total}, prevRange_.total);
  auto retHigh =
      impl::multiply_decrease_and_divide(
          prevRange_.high, TmpRange_::WideNum{RC::total}, prevRange_.total) +
      1;
  return {static_cast<RC::Count>(retLow), static_cast<RC::Count>(retHigh)};
}

}  // namespace ael

#endif  // AEL_ARITHMETIC_CODER_HPP
