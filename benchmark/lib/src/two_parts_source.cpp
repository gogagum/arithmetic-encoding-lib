#include <two_parts_source.hpp>

////////////////////////////////////////////////////////////////////////////////
TwoPartsSource::GenerationInstance::Iterator_::Iterator_(
    GenerationInstance* ownerPtr, std::size_t offset)
    : offset_{offset}, ownerPtr_{ownerPtr} {};

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::GenerationInstance::Iterator_::operator*() const
    -> std::uint64_t {
  return ownerPtr_->get_();
}

////////////////////////////////////////////////////////////////////////////////
bool TwoPartsSource::GenerationInstance::Iterator_::operator!=(
    const Iterator_& other) const {
  return offset_ != other.offset_ || ownerPtr_ != other.ownerPtr_;
}

////////////////////////////////////////////////////////////////////////////////
bool TwoPartsSource::GenerationInstance::Iterator_::operator==(
    const Iterator_& other) const {
  return offset_ == other.offset_ && ownerPtr_ == other.ownerPtr_;
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::GenerationInstance::Iterator_::operator++() -> Iterator_& {
  ++offset_;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::GenerationInstance::Iterator_::operator++(int)
    -> Iterator_ {
  const auto ret = Iterator_(ownerPtr_, offset_);
  ++offset_;
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSource::getMinH(std::uint64_t maxOrd, std::uint64_t m) {
  return enthropy_(0, maxOrd, m);
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSource::getMaxH(std::uint64_t maxOrd, std::uint64_t m) {
  const auto pMax = static_cast<double>(m) / static_cast<double>(maxOrd);
  return enthropy_(pMax, maxOrd, m);
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
double TwoPartsSource::enthropy_(    // NOLINT
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
    : m_(constructInfo.m),
      maxOrd_(constructInfo.maxOrd),
      length_(constructInfo.length),
      p_(calcP_(constructInfo.h, maxOrd_, m_)),
      generator_(constructInfo.seed) {
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::GenerationInstance::begin() -> Iterator_ {
  return {this, 0};
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::GenerationInstance::end() -> Iterator_ {
  return {this, length_};
}

////////////////////////////////////////////////////////////////////////////////
std::size_t TwoPartsSource::GenerationInstance::size() const {
  return length_;
}

////////////////////////////////////////////////////////////////////////////////
std::uint64_t TwoPartsSource::GenerationInstance::get_() {
  constexpr auto ticks = std::uint64_t{1 << 16};
  const auto part = std::uint64_t{generator_() % ticks};
  if (static_cast<double>(part) / static_cast<double>(ticks) < p_) {
    return generator_() % m_;
  }
  return m_ + generator_() % (maxOrd_ - m_);
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSource::calcP_(double h, std::uint64_t maxOrd, std::uint64_t m) {
  const auto pMax = static_cast<double>(m) / static_cast<double>(maxOrd);
  if (h < std::log2(m) || h > std::log2(maxOrd)) {
    throw std::invalid_argument(fmt::format(
        "H = {} is not reachable with m = {}, M = {}. "
        "Minimal H is {}, maximal H is {}",
        h, m, maxOrd, enthropy_(0, maxOrd, m), enthropy_(pMax, maxOrd, m)));
  }
  double pL = 0;
  auto pR = pMax;
  constexpr auto half = double{0.5};
  if (pMax < half) {
    pL = 1;
    pR = pMax;
  }
  while (std::abs(pR - pL) > std::numeric_limits<double>::epsilon()) {
    if (const double pM = (pL + pR) / 2; enthropy_(pM, maxOrd, m) > h) {
      pR = pM;
    } else {
      pL = pM;
    }
  }
  return pL;
}
