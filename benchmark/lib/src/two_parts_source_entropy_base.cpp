#include <impl/two_parts_source_entropy_base.hpp>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <fmt/format.h>

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceEntropyBase::getMinEntropy(std::uint64_t maxOrd, std::uint64_t m) {
  return (2 * m > maxOrd) ? entropy(0, maxOrd, m) : entropy(1, maxOrd, m);
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceEntropyBase::getMaxEntropy(std::uint64_t maxOrd, std::uint64_t m) {
  const auto pMax = static_cast<double>(m) / static_cast<double>(maxOrd);
  return entropy(pMax, maxOrd, m);
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceEntropyBase::getMinMaxEntropy(std::uint64_t maxOrd, std::uint64_t m)
    -> EntropyRange {
  return {getMinEntropy(maxOrd, m), getMaxEntropy(maxOrd, m)};
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceEntropyBase::entropy(     // NOLINT
    double p, std::uint64_t maxOrd,  // NOLINT
    std::uint64_t m) {               // NOLINT
  const auto mDouble = static_cast<double>(m);
  const auto maxOrdDouble = static_cast<double>(maxOrd);
  if (std::abs(p - 1) < std::numeric_limits<double>::epsilon()) {
    return std::log2(mDouble);
  }
  if (p < std::numeric_limits<double>::epsilon()) {
    return std::log2(maxOrdDouble - mDouble);
  }
  return -p * std::log2(p / mDouble) -
         (1 - p) * std::log2((1 - p) / (maxOrdDouble - mDouble));
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceEntropyBase::calcP_(double h, std::uint64_t maxOrd, std::uint64_t m) {
  const auto maxEntropy = getMaxEntropy(maxOrd, m);
  const auto minEntropy = getMinEntropy(maxOrd, m);
  if (h < minEntropy || h > maxEntropy) {
    throw std::invalid_argument(
        fmt::format("H(X) = {} is not reachable with m = {}, M = {}. "
                    "Minimal H(X) is {}, maximal H is {}",
                    h, m, maxOrd, minEntropy, maxEntropy));
  }
  auto pL = (2 * m > maxOrd) ? double{0} : double{1};
  auto pR = static_cast<double>(m) / static_cast<double>(maxOrd);
  const auto entropy0 = [maxOrd, m](double p) -> double {
    return entropy(p, maxOrd, m);
  };
  while (std::abs(pR - pL) > std::numeric_limits<double>::epsilon()) {
    if (const double pM = (pL + pR) / 2; entropy0(pM) > h) {
      pR = pM;
    } else {
      pL = pM;
    }
  }
  return pL;
}
