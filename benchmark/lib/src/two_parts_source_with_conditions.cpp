#include <fmt/format.h>

#include <iterator>
#include <limits>
#include <stdexcept>
#include <two_parts_source_with_conditions.hpp>

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceWithConditions::getMinHXX(double p, std::uint64_t maxOrd,
                                               std::uint64_t m) {
  constexpr auto half = double{0.5};
  return (p < half) ? entropyWithCondition_(p, -p * p, maxOrd, m)
                    : entropyWithCondition_(p, p * (1 - p), maxOrd, m);
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceWithConditions::getMaxHXX(double p, std::uint64_t maxOrd,
                                               std::uint64_t m) {
  return entropy_(p, maxOrd, m);
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceWithConditions::getMinMaxHXX(double p, std::uint64_t maxOrd,
                                                std::uint64_t m)
    -> GenerationInstance::HRange {
  return {getMinHXX(p, maxOrd, m), getMaxHXX(p, maxOrd, m)};
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceWithConditions::getGeneration(
    GenerationConfig generationConfig) -> GenerationInstance {
  if (generationConfig.m == 0 ||
      generationConfig.m >= generationConfig.maxOrd) {
    throw std::invalid_argument(
        fmt::format("m = {} has no logic.", generationConfig.m));
  }
  return GenerationInstance({generationConfig.maxOrd, generationConfig.m,
                             generationConfig.h, generationConfig.hxx,
                             generationConfig.length, generationConfig.seed});
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceWithConditions::entropyWithCondition_(double p,
                                                           double delta,
                                                           std::uint64_t maxOrd,
                                                           std::uint64_t m) {
  const double q = 1 - p;
  const auto mD = static_cast<double>(m);
  const double mu = static_cast<double>(maxOrd) - mD;
  const double pSquared = p * p;
  const double qSquared = q * q;
  const auto mSquared = static_cast<double>(m * m);
  const auto muSquared = static_cast<double>(mu * mu);
  if (std::abs(pSquared + delta) < std::numeric_limits<double>::epsilon()) {
    return p * std::log2(mu) + q * std::log2(mD);
  }
  if (std::abs(qSquared + delta) < std::numeric_limits<double>::epsilon()) {
    return -p * std::log2((p - q) / (mD * p)) -
           q * std::log2(q / (mu * (p - q)));
  }
  if (std::abs(p * q - delta) < std::numeric_limits<double>::epsilon()) {
    return p * std::log2(mD) + q * std::log2(mu);
  }
  return -pSquared * std::log2((pSquared + delta) / mSquared) -
         2 * p * q * std::log2((p * q - delta) / (mD * mu)) -
         qSquared * std::log2((qSquared + delta) / muSquared) -
         delta * std::log2((pSquared + delta) * (qSquared + delta)) +
         2 * delta * std::log2(p * q - delta) +
         p * std::log2(p / mD) + q * std::log2(q / mu);
}

////////////////////////////////////////////////////////////////////////////////
TwoPartsSourceWithConditions::GenerationInstance::GenerationInstance(
    ConstructInfo constructInfo)
    : SubrangeBase_(std::counted_iterator(
                        Iterator_(*this),
                        static_cast<std::ptrdiff_t>(constructInfo.length)),
                    std::default_sentinel),
      m_(constructInfo.m),
      maxOrd_(constructInfo.maxOrd),
      p_(calcP_(constructInfo.h, maxOrd_, m_)),
      delta_(calcDelta_(constructInfo.hxx, maxOrd_, m_, p_, constructInfo.h)),
      generator_(constructInfo.seed) {
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

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceWithConditions::calcDelta_(double hxx,
                                                std::uint64_t maxOrd,
                                                std::uint64_t m, double p,
                                                double h) {
  const auto deltaMax = double{0};
  const auto maxEntropyWithCondition = h;
  const auto minEntropyWithCondition = getMinHXX(p, maxOrd, m);
  if (hxx < minEntropyWithCondition || hxx > maxEntropyWithCondition) {
    throw std::invalid_argument(fmt::format(
        "H(X|X) = {} is not reachable with m = {}, M = {}. Minimal "
        "H(X|X) is {}, maximal H(X|X) is {}",
        hxx, m, maxOrd, minEntropyWithCondition, maxEntropyWithCondition));
  }
  constexpr auto half = double{0.5};
  double deltaL = (p < half) ? -p * p : p * (1 - p);
  double deltaR = deltaMax;
  const auto entropyWithCondition = [p, maxOrd, m](double delta) -> double {
    return entropyWithCondition_(p, delta, maxOrd, m);
  };
  while (std::abs(deltaR - deltaL) > std::numeric_limits<double>::epsilon()) {
    if (const double deltaM = (deltaL + deltaR) / 2;
        entropyWithCondition(deltaM) > hxx) {
      deltaR = deltaM;
    } else {
      deltaL = deltaM;
    }
  }
  return deltaL;
}
