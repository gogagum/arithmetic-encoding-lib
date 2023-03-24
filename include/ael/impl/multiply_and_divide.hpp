#ifndef MULTIPLY_AND_DIVIDE_HPP
#define MULTIPLY_AND_DIVIDE_HPP

#include <boost/multiprecision/cpp_int/cpp_int_config.hpp>
#include <cstddef>
#include <cstdint>
#include <boost/multiprecision/cpp_int.hpp>

namespace ael::impl {

namespace bm = boost::multiprecision;

/**
 * @brief multiply_and_divide calculate (l * r) / d assuming that result is
 * under 64 bits.
 * @param l
 * @param r
 * @param d
 * @return (l * r) / d
 */
std::uint64_t multiply_and_divide(std::uint64_t l,
                                  std::uint64_t r,
                                  std::uint64_t d);

/**
 * @brief multiply_decrease_and_divide calculate (l * r - 1) / d assuming that result is
 * under 64 bits.
 * @param l
 * @param r
 * @param d
 * @return (l * r - 1) / d
 */
std::uint64_t multiply_decrease_and_divide(std::uint64_t l,
                                           std::uint64_t r,
                                           std::uint64_t d);


template<std::size_t numBits>
using WideNum = bm::number<bm::cpp_int_backend<
    numBits,
    numBits,
    bm::unsigned_magnitude,
    bm::unchecked, void>>;

/**
 * @brief multiply_and_divide calculate (l * r) / d assuming that result is
 * ok for a type of inputs.
 * @param l
 * @param r
 * @param d
 * @return (l * r) / d
 */
template<std::size_t numBits>
WideNum<numBits> multiply_and_divide(const WideNum<numBits>& l,
                                     const WideNum<numBits>& r,
                                     const WideNum<numBits>& d) {
    assert(d != 0);
    return (
            (WideNum<numBits * 2>{l} * WideNum<numBits * 2>{r})
            / WideNum<numBits * 2>{d}
        ).template convert_to<WideNum<numBits>>(); 
}

/**
 * @brief multiply_and_divide calculate (l * r - 1) / d assuming that result is
 * ok for a type of inputs.
 * @param l
 * @param r
 * @param d
 * @return (l * r - 1) / d
 */
template<std::size_t numBits>
WideNum<numBits> multiply_decrease_and_divide(const WideNum<numBits>& l,
                                              const WideNum<numBits>& r,
                                              const WideNum<numBits>& d) {
    assert(d != 0);
    return (
            (WideNum<numBits * 2>{l} * WideNum<numBits * 2>{r} - 1)
            / WideNum<numBits * 2>{d}
        ).template convert_to<WideNum<numBits>>(); 
}

}  // namespace ael::impl

#endif
