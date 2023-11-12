#include <fmt/format.h>

#include <cmath>
#include <impl/two_parts_source_conditional_entropy_base.hpp>
#include <impl/two_parts_source_entropy_base.hpp>
#include <limits>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
double SourceConditionalEntropyBase::getMinConditionalEntropy(
    double p, std::uint64_t maxOrd, std::uint64_t m) {
  constexpr auto half = double{0.5};
  return (p < half) ? entropyWithCondition(p, -p * p, maxOrd, m)
                    : entropyWithCondition(p, p * (1 - p), maxOrd, m);
}

////////////////////////////////////////////////////////////////////////////////
double SourceConditionalEntropyBase::getMaxConditionalEntropy(
    double p, std::uint64_t maxOrd, std::uint64_t m) {
  return TwoPartsSourceEntropyBase::entropy(p, maxOrd, m);
}

////////////////////////////////////////////////////////////////////////////////
auto SourceConditionalEntropyBase::getMinMaxConditionalEntropy(
    double p, std::uint64_t maxOrd, std::uint64_t m)
    -> ConditionalEntropyRange {
  return {getMinConditionalEntropy(p, maxOrd, m),
          getMaxConditionalEntropy(p, maxOrd, m)};
}

////////////////////////////////////////////////////////////////////////////////
double SourceConditionalEntropyBase::entropyWithCondition(double p,
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
         2 * delta * std::log2(p * q - delta) + p * std::log2(p / mD) +
         q * std::log2(q / mu);
}

////////////////////////////////////////////////////////////////////////////////
double SourceConditionalEntropyBase::calcDelta_(double hxx,
                                                std::uint64_t maxOrd,
                                                std::uint64_t m, double p,
                                                double h) {
  const auto deltaMax = double{0};
  const auto maxEntropyWithCondition = h;
  const auto minEntropyWithCondition = getMinConditionalEntropy(p, maxOrd, m);
  if (hxx < minEntropyWithCondition || hxx > maxEntropyWithCondition) {
    throw std::invalid_argument(fmt::format(
        "H(X|X) = {} is not reachable with m = {}, M = {}. Minimal "
        "H(X|X) is {}, maximal H(X|X) is {}",
        hxx, m, maxOrd, minEntropyWithCondition, maxEntropyWithCondition));
  }
  constexpr auto half = double{0.5};
  double deltaL = (p < half) ? -p * p : p * (1 - p);
  double deltaR = deltaMax;
  const auto entropyWithCondition0 = [p, maxOrd, m](double delta) -> double {
    return entropyWithCondition(p, delta, maxOrd, m);
  };
  while (std::abs(deltaR - deltaL) > std::numeric_limits<double>::epsilon()) {
    if (const double deltaM = (deltaL + deltaR) / 2;
        entropyWithCondition0(deltaM) > hxx) {
      deltaR = deltaM;
    } else {
      deltaL = deltaM;
    }
  }
  return deltaL;
}
