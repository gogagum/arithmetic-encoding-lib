#include <fmt/format.h>

#include <iterator>
#include <limits>
#include <stdexcept>
#include <two_parts_source_with_conditions.hpp>

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceWithConditions::getGeneration(std::size_t maxOrd,
                                                 std::size_t m, double h,
                                                 double hxx, std::size_t length,
                                                 std::uint64_t seed)
    -> GenerationInstance {
  if (m == 0 || m >= maxOrd) {
    throw std::invalid_argument(fmt::format("m = {} has no logic.", m));
  }
  return GenerationInstance({maxOrd, m, h, hxx, length, seed});
}

////////////////////////////////////////////////////////////////////////////////
TwoPartsSourceWithConditions::GenerationInstance::GenerationInstance(
    std::size_t maxOrd, std::size_t m, double h, double hxx, std::size_t length,
    std::uint64_t seed)
    : SubrangeBase_(std::counted_iterator(Iterator_(*this),
                                          static_cast<std::ptrdiff_t>(length)),
                    std::default_sentinel),
      m_(m),
      maxOrd_(maxOrd),
      p_(calcP_(h, maxOrd_, m_)),
      delta_(calcDelta_(hxx, maxOrd_, m_, p_, h)),
      generator_(seed) {
}

////////////////////////////////////////////////////////////////////////////////
std::uint64_t TwoPartsSourceWithConditions::GenerationInstance::get_() {
  using UniformDistr = std::uniform_int_distribution<std::uint64_t>;
  if (!prevGenerated_.has_value()) [[unlikely]] {
    auto partChoice = std::bernoulli_distribution(p_);
    auto finalDistr = partChoice(generator_) ? UniformDistr(0, m_ - 1)
                                             : UniformDistr(m_, maxOrd_ - 1);
    prevGenerated_ = finalDistr(generator_);
    return *prevGenerated_;
  }
  if (*prevGenerated_ < m_) {
    auto partChoice = std::bernoulli_distribution(p_ + delta_ / p_);
    auto finalDistr = partChoice(generator_) ? UniformDistr(0, m_ - 1)
                                             : UniformDistr(m_, maxOrd_ - 1);
    prevGenerated_ = finalDistr(generator_);
    return *prevGenerated_;
  } else {
    auto partChoice = std::bernoulli_distribution(p_ - delta_ / (1.0 - p_));
    auto finalDistr = partChoice(generator_) ? UniformDistr(0, m_ - 1)
                                             : UniformDistr(m_, maxOrd_ - 1);
    prevGenerated_ = finalDistr(generator_);
    return *prevGenerated_;
  }
}
