#include <ael/numerical_coder.hpp>

namespace ael{

////////////////////////////////////////////////////////////////////////////////
NumericalCoder::NumericalCoder(
    std::unique_ptr<ByteDataConstructor>&& dataConstructor)
    : dataConstructor_{std::move(dataConstructor)} {
}

}  // namespace ael
