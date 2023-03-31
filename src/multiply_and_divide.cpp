#include <ael/impl/multiply_and_divide.hpp>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
std::uint64_t multiply_and_divide(std::uint64_t left, std::uint64_t right,
                                  std::uint64_t divider) {
  return (bm::uint128_t{left} * bm::uint128_t{right} / divider)
      .convert_to<std::uint64_t>();
}

////////////////////////////////////////////////////////////////////////////////
std::uint64_t multiply_decrease_and_divide(std::uint64_t left,
                                           std::uint64_t right,
                                           std::uint64_t divider) {
  return ((bm::uint128_t{left} * bm::uint128_t{right} - 1) / divider)
      .convert_to<std::uint64_t>();
}

}  // namespace ael::impl
