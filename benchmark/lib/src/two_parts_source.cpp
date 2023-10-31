#include <fmt/format.h>

#include <two_parts_source.hpp>

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSource::getMinH(std::uint64_t maxOrd, std::uint64_t m) {
  return entropy_(0, maxOrd, m);
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSource::getMaxH(std::uint64_t maxOrd, std::uint64_t m) {
  const auto pMax = static_cast<double>(m) / static_cast<double>(maxOrd);
  return entropy_(pMax, maxOrd, m);
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::getMinMaxH(std::uint64_t maxOrd, std::uint64_t m)
    -> GenerationInstance::HRange {
  return {getMinH(maxOrd, m), getMaxH(maxOrd, m)};
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::getGeneration(GenerationConfig generationConfig)
    -> GenerationInstance {
  if (generationConfig.m == 0 ||
      generationConfig.m >= generationConfig.maxOrd) {
    throw std::invalid_argument(
        fmt::format("m = {} has no logic.", generationConfig.m));
  }
  return GenerationInstance({generationConfig.maxOrd, generationConfig.m,
                             generationConfig.h, generationConfig.length,
                             generationConfig.seed});
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSource::entropy_(     // NOLINT
    double p, std::uint64_t maxOrd,  // NOLINT
    std::uint64_t m) {               // NOLINT
  const auto mDouble = static_cast<double>(m);
  const auto maxOrdDouble = static_cast<double>(maxOrd);
  if (std::abs(p - 1) < std::numeric_limits<double>::epsilon()) {
    return -p * std::log2(p / mDouble);
  }
  if (p < std::numeric_limits<double>::epsilon()) {
    return -(1 - p) * std::log2((1 - p) / (maxOrdDouble - mDouble));
  }
  return -p * std::log2(p / mDouble) -
         (1 - p) * std::log2((1 - p) / (maxOrdDouble - mDouble));
}

////////////////////////////////////////////////////////////////////////////////
TwoPartsSource::GenerationInstance::GenerationInstance(
    ConstructInfo constructInfo)
    : SubrangeBase_(std::counted_iterator(
                        Iterator_(*this),
                        static_cast<std::ptrdiff_t>(constructInfo.length)),
                    std::default_sentinel),
      m_(constructInfo.m),
      maxOrd_(constructInfo.maxOrd),
      p_(calcP_(constructInfo.h, maxOrd_, m_)),
      generator_(constructInfo.seed) {
}

////////////////////////////////////////////////////////////////////////////////
std::uint64_t TwoPartsSource::GenerationInstance::get_() {
  constexpr auto ticks = std::uint64_t{1 << 16};
  if (const auto part = static_cast<double>(generator_() % ticks);
      part / static_cast<double>(ticks) < p_) {
    return generator_() % m_;
  }
  return m_ + generator_() % (maxOrd_ - m_);
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSource::calcP_(double h, std::uint64_t maxOrd, std::uint64_t m) {
  const auto pMax = static_cast<double>(m) / static_cast<double>(maxOrd);
  const auto maxEntropy = entropy_(pMax, maxOrd, m);
  const auto minEntropy =
      std::min(entropy_(0, maxOrd, m), entropy_(0, maxOrd, maxOrd - m));
  if (h < minEntropy || h > maxEntropy) {
    throw std::invalid_argument(
        fmt::format("H(X) = {} is not reachable with m = {}, M = {}. "
                    "Minimal H(X) is {}, maximal H is {}",
                    h, m, maxOrd, minEntropy, maxEntropy));
  }
  constexpr auto half = double{0.5};
  auto pL = (pMax < half) ? double{1} : double{0};
  auto pR = pMax;
  const auto entropy = [maxOrd, m](double p) -> double {
    return entropy_(p, maxOrd, m);
  };
  while (std::abs(pR - pL) > std::numeric_limits<double>::epsilon()) {
    if (const double pM = (pL + pR) / 2; entropy(pM) > h) {
      pR = pM;
    } else {
      pL = pM;
    }
  }
  return pL;
}
