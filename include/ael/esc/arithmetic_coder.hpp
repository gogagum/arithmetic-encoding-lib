#ifndef AEL_ESC_ARITHMETIC_CODER_HPP
#define AEL_ESC_ARITHMETIC_CODER_HPP

#include <ael/byte_data_constructor.hpp>
#include <ael/impl/ranges_calc.hpp>
#include <cstdint>

namespace ael::esc {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
class ArithmeticCoder {
 public:
  struct EncodeRet {
    std::size_t wordsCount;
    std::size_t bitsEncoded;
  };

 public:
  /**
   * @brief encode - encode byte flow.
   * @param ordFlow - orders range.
   * @param dataConstructor encoded sequence constructor.
   * @param dict dictionary for words stats in esc-style.
   * @return [wordsCount, bitsEncoded]
   */
  template <class DictT>
  [[nodiscard]] static EncodeRet encode(auto ordFlow,
                                        ByteDataConstructor& dataConstructor,
                                        DictT& dict);

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
  [[nodiscard]] static EncodeRet encode(auto ordFlow,
                                        ByteDataConstructor& dataConstructor,
                                        DictT& dict, auto tick);
};

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
auto ArithmeticCoder::encode(auto ordFlow, ByteDataConstructor& dataConstructor,
                             DictT& dict) -> EncodeRet {
  return encode(ordFlow, dataConstructor, dict, [] {});
}

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
auto ArithmeticCoder::encode(auto ordFlow, ByteDataConstructor& dataConstructor,
                             DictT& dict, auto tick) -> EncodeRet {
  auto ret = EncodeRet();
  using RC = impl::RangesCalc<typename DictT::Count, DictT::countNumBits>;
  auto currRange = typename RC::Range{0, RC::total};

  auto btf = std::size_t{0};

  for (auto ord : ordFlow) {
    const auto statsSeq = dict.getProbabilityStats(ord);
    for (const auto stats : statsSeq) {
      const auto [low, high, total] = stats;
      currRange = RC::rangeFromStatsAndPrev(currRange, {low, high, total});

      while (true) {
        if (currRange.high <= RC::half) {
          ret.bitsEncoded += btf + 1;
          dataConstructor.putBit(false);
          dataConstructor.putBitsRepeatWithReset(true, btf);
        } else if (currRange.low >= RC::half) {
          ret.bitsEncoded += btf + 1;
          dataConstructor.putBit(true);
          dataConstructor.putBitsRepeatWithReset(false, btf);
        } else if (currRange.low >= RC::quater &&
                   currRange.high <= RC::threeQuaters) {
          ++btf;
        } else {
          break;
        }
        currRange = RC::recalcRange(currRange);
      }
      ++ret.wordsCount;
    }
    tick();
  }

  ret.bitsEncoded += btf + 2;
  if (currRange.low < RC::quater) {
    dataConstructor.putBit(false);
    dataConstructor.putBitsRepeat(true, btf + 1);
  } else {
    dataConstructor.putBit(true);
    dataConstructor.putBitsRepeat(false, btf + 1);
  }

  return ret;
}

}  // namespace ael::esc

#endif  // AEL_ESC_ARITHMETIC_CODER_HPP
