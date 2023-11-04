#include <ael/arithmetic_coder.hpp>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
auto ArithmeticCoder::getStatsChange() -> Stats {
  auto ret = Stats{wordsCnt_ - prevWordsCnt_, bitsEncoded_ - prevBitsEncoded_};
  prevWordsCnt_ = wordsCnt_;
  prevBitsEncoded_ = bitsEncoded_;
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto ArithmeticCoder::finalize() && -> FinalRet {
  bitsEncoded_ += btf_ + 2;
  dataConstructor_->putBit(!finalizeChoice_);
  dataConstructor_->putBitsRepeat(finalizeChoice_, btf_ + 1);

  return {std::move(dataConstructor_), wordsCnt_, bitsEncoded_};
}

}  // namespace ael
