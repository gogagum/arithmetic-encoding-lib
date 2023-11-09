#ifndef AEL_IMPL_BYTE_RNG_TO_BITS_HPP
#define AEL_IMPL_BYTE_RNG_TO_BITS_HPP

#include <climits>
#include <ranges>

namespace ael::impl {

/**
 * @brief get bits view over std::byte sized range.
 * 
 * @tparam Rng std::byte sized range type
 * @param rng std::byte sized range
 * @return bits view
 */
template <std::ranges::sized_range Rng>
  requires std::is_same_v<std::ranges::range_value_t<Rng>, std::byte>
auto to_bits(const Rng& rng) {
  return std::views::iota(std::size_t{0}, rng.size() * CHAR_BIT) |
         std::views::transform([&rng](std::size_t i) -> bool {
           const auto byte_idx = i / CHAR_BIT;
           const auto bit_idx = 7 - i % CHAR_BIT;
           return ((rng[byte_idx] >> bit_idx) & std::byte{1}) == std::byte{1};
         });
}

}  // namespace ael::impl

#endif  // AEL_IMPL_BYTE_RNG_TO_BITS_HPP
