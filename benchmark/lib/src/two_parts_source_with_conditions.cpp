#include <fmt/format.h>

#include <limits>
#include <stdexcept>
#include <two_parts_source_with_conditions.hpp>

////////////////////////////////////////////////////////////////////////////////
TwoPartsSourceWithConditions::GenerationInstance::Iterator_::Iterator_(
    GenerationInstance& ownerPtr, std::size_t offset)
    : offset_{offset}, ownerPtr_{&ownerPtr} {};

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceWithConditions::GenerationInstance::Iterator_::operator*() const
    -> std::uint64_t {
  return ownerPtr_->get_();
}

////////////////////////////////////////////////////////////////////////////////
bool TwoPartsSourceWithConditions::GenerationInstance::Iterator_::operator!=(
    const Iterator_& other) const {
  return offset_ != other.offset_ || ownerPtr_ != other.ownerPtr_;
}

////////////////////////////////////////////////////////////////////////////////
bool TwoPartsSourceWithConditions::GenerationInstance::Iterator_::operator==(
    const Iterator_& other) const {
  return offset_ == other.offset_ && ownerPtr_ == other.ownerPtr_;
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceWithConditions::GenerationInstance::Iterator_::operator++() -> Iterator_& {
  ++offset_;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceWithConditions::GenerationInstance::Iterator_::operator++(int)
    -> Iterator_ {
  const auto ret = *this;
  ++offset_;
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceWithConditions::getMinH(std::uint64_t maxOrd,
                                             std::uint64_t m) {
  return std::min(entropy_(0, maxOrd, m), entropy_(1, maxOrd, m));
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceWithConditions::getMaxH(std::uint64_t maxOrd,
                                             std::uint64_t m) {
  const auto pMax = static_cast<double>(m) / static_cast<double>(maxOrd);
  return entropy_(pMax, maxOrd, m);
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceWithConditions::getMinMaxH(std::uint64_t maxOrd,
                                              std::uint64_t m)
    -> GenerationInstance::HRange {
  return {getMinH(maxOrd, m), getMaxH(maxOrd, m)};
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceWithConditions::getMinHXX(std::uint64_t maxOrd,
                                               std::uint64_t m) {
  return 0; // TODO(gogagum)
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceWithConditions::getMaxHXX(std::uint64_t maxOrd,
                                               std::uint64_t m) {
  return 0; // TODO(gogagum)
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceWithConditions::getMinMaxHXX(std::uint64_t maxOrd,
                                                std::uint64_t m)
    -> GenerationInstance::HRange {
  return {getMinHXX(maxOrd, m), getMaxHXX(maxOrd, m)};
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
double TwoPartsSourceWithConditions::entropy_(  // NOLINT
    double p, std::uint64_t maxOrd,             // NOLINT
    std::uint64_t m) {                          // NOLINT
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
double TwoPartsSourceWithConditions::entropyWithCondition_(double p,
                                                           double delta,
                                                           std::uint64_t maxOrd,
                                                           std::uint64_t m) {
  // TODO(gogagum)
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
TwoPartsSourceWithConditions::GenerationInstance::GenerationInstance(
    ConstructInfo constructInfo)
    : m_(constructInfo.m),
      maxOrd_(constructInfo.maxOrd),
      length_(constructInfo.length),
      p_(calcP_(constructInfo.h, maxOrd_, m_)),
      delta_(calcDelta_(constructInfo.hxx, maxOrd_, m_, p_, constructInfo.h)),
      generator_(constructInfo.seed) {
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceWithConditions::GenerationInstance::begin() -> Iterator_ {
  return {*this, 0};
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSourceWithConditions::GenerationInstance::end() -> Iterator_ {
  return {*this, length_};
}

////////////////////////////////////////////////////////////////////////////////
std::size_t TwoPartsSourceWithConditions::GenerationInstance::size() const {
  return length_;
}

////////////////////////////////////////////////////////////////////////////////
std::uint64_t TwoPartsSourceWithConditions::GenerationInstance::get_() {
  constexpr auto ticks = std::uint64_t{1 << 16};
  if (!prevGenerated_.has_value()) [[unlikely]] {
    if (const auto part = static_cast<double>(generator_() % ticks);
        part / static_cast<double>(ticks) < p_) {
      return generator_() % m_;
    }
    prevGenerated_ = m_ + generator_() % (maxOrd_ - m_);
    return *prevGenerated_;
  }
  if (*prevGenerated_ < m_) {
    if (const auto part = static_cast<double>(generator_() % ticks);
        part / static_cast<double>(ticks) < p_ + delta_ / p_) {
      prevGenerated_ = generator_() % m_;
      return *prevGenerated_;
    }
    prevGenerated_ = m_ + generator_() % (maxOrd_ - m_);
    return *prevGenerated_;
  } else {
    if (const auto part = static_cast<double>(generator_() % ticks);
        part / static_cast<double>(ticks) >= (1 - p_) - delta_ / (1 - p_)) {
      prevGenerated_ = generator_() % m_;
      return *prevGenerated_;
    }
    prevGenerated_ = m_ + generator_() % (maxOrd_ - m_);
    return *prevGenerated_;
  }
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceWithConditions::calcP_(double h, std::uint64_t maxOrd,
                                            std::uint64_t m) {
  const auto pMax = static_cast<double>(m) / static_cast<double>(maxOrd);
  const auto maxEntropy = entropy_(pMax, maxOrd, m);
  const auto minEntropy =
      std::min(entropy_(0, maxOrd, m), entropy_(0, maxOrd, maxOrd - m));
  if (h < minEntropy || h > maxEntropy) {
    throw std::invalid_argument(
        fmt::format("H = {} is not reachable with m = {}, M = {}. "
                    "Minimal H is {}, maximal H is {}",
                    h, m, maxOrd, minEntropy, maxEntropy));
  }
  constexpr auto half = double{0.5};
  double pL = (pMax < half) ? 1 : 0;
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

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSourceWithConditions::calcDelta_(double hxx,
                                                std::uint64_t maxOrd,
                                                std::uint64_t m, double p,
                                                double h) {
  const auto pMax = static_cast<double>(m) / static_cast<double>(maxOrd);
  const auto deltaMax = double{0};
  const auto maxEntropyWithCondition = h;
  const auto minEntropyWithCondition =
      std::min(entropyWithCondition_(p, -p * p, maxOrd, m),
               entropyWithCondition_(p, -(1 - p) * (1 - p), maxOrd, m));
  if (hxx < minEntropyWithCondition || hxx > maxEntropyWithCondition) {
    throw std::invalid_argument(fmt::format(
        "H(X|X) = {} is not reachable with m = {}, M = {}. Minimal "
        "H(X|X) is {}, maximal H(X|X) is {}",
        hxx, m, maxOrd, minEntropyWithCondition, maxEntropyWithCondition));
  }
  constexpr auto half = double{0.5};
  double deltaL = (pMax < half) ? -(1 - p) * (1 - p) : -p * p;
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
