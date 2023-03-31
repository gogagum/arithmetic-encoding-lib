#ifndef MULTIPLY_AND_DIVIDE_HPP
#define MULTIPLY_AND_DIVIDE_HPP

#include <boost/multiprecision/cpp_int.hpp>
#include <cstddef>
#include <cstdint>

namespace ael::impl {

namespace bm = boost::multiprecision;

/**
 * @brief multiply_and_divide calculate (left * right) / divider assuming that
 * result is under 64 bits.
 * @param left
 * @param right
 * @param divider
 * @return (left * right) / divider
 */
std::uint64_t multiply_and_divide(std::uint64_t left, std::uint64_t right,
                                  std::uint64_t divider);

/**
 * @brief multiply_decrease_and_divide calculate (left * right - 1) / divider
 * assuming that result is under 64 bits.
 * @param left
 * @param right
 * @param ddivider
 * @return (left * right - 1) / divider
 */
std::uint64_t multiply_decrease_and_divide(std::uint64_t left,
                                           std::uint64_t right,
                                           std::uint64_t divider);

template <std::size_t numBits>
using WideNum =
    bm::number<bm::cpp_int_backend<numBits, numBits, bm::unsigned_magnitude,
                                   bm::unchecked, void>>;

/**
 * @brief multiply_and_divide calculate (left * right) / divider assuming that
 * result is ok for a type of inputs.
 * @param left
 * @param right
 * @param divider
 * @return (left * right) / divider
 */
template <std::size_t numBits>
WideNum<numBits> multiply_and_divide(const WideNum<numBits>& left,
                                     const WideNum<numBits>& right,
                                     const WideNum<numBits>& divider) {
  assert(divider != 0 && "Division by zero.");
  return ((WideNum<numBits * 2>{left} * WideNum<numBits * 2>{right}) /
          WideNum<numBits * 2>{divider})
      .template convert_to<WideNum<numBits>>();
}

/**
 * @brief multiply_and_divide calculate (l * r - 1) / d assuming that result is
 * ok for a type of inputs.
 * @param l
 * @param r
 * @param d
 * @return (l * r - 1) / d
 */
template <std::size_t numBits>
WideNum<numBits> multiply_decrease_and_divide(const WideNum<numBits>& left,
                                              const WideNum<numBits>& right,
                                              const WideNum<numBits>& divider) {
  assert(divider != 0);
  return ((WideNum<numBits * 2>{left} * WideNum<numBits * 2>{right} - 1) /
          WideNum<numBits * 2>{divider})
      .template convert_to<WideNum<numBits>>();
}

}  // namespace ael::impl

#endif
