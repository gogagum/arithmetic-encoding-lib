#include <ael/arithmetic_coder.hpp>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
auto ArithmeticCoder::finalize() && -> EncodeRet {
  ret_.bitsEncoded += btf_ + 2;
  if (finalizeChoice_) {
    getEncoded_().putBit(false);
    getEncoded_().putBitsRepeat(true, btf_ + 1);
  } else {
    getEncoded_().putBit(true);
    getEncoded_().putBitsRepeat(false, btf_ + 1);
  }

  return std::move(ret_);
}

}
