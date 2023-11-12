#include <fmt/format.h>

#include <random>
#include <stdexcept>
#include <two_parts_source.hpp>

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::getGeneration(std::size_t maxOrd, std::size_t m, double h,
                                   std::size_t length, std::uint64_t seed)
    -> GenerationInstance {
  if (m == 0 || m >= maxOrd) {
    throw std::invalid_argument(fmt::format("m = {} has no logic.", m));
  }
  return GenerationInstance(maxOrd, m, h, length, seed);
}

////////////////////////////////////////////////////////////////////////////////
TwoPartsSource::GenerationInstance::GenerationInstance(std::size_t maxOrd,
                                                       std::size_t m, double h,
                                                       std::size_t length,
                                                       std::uint64_t seed)
    : SubrangeBase_(std::counted_iterator(Iterator_(*this),
                                          static_cast<std::ptrdiff_t>(length)),
                    std::default_sentinel),
      m_(m),
      maxOrd_(maxOrd),
      p_(calcP_(h, maxOrd_, m_)),
      generator_(seed),
      partChoice_(p_) {
}

////////////////////////////////////////////////////////////////////////////////
std::uint64_t TwoPartsSource::GenerationInstance::get_() {
  using UniformDistr = std::uniform_int_distribution<std::uint64_t>;
  auto finalDistr = partChoice_(generator_) ? UniformDistr(0, m_ - 1)
                                            : UniformDistr(m_, maxOrd_ - 1);
  return finalDistr(generator_);
}
