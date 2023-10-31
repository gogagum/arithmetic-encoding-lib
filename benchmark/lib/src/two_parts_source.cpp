#include <fmt/format.h>

#include <random>
#include <two_parts_source.hpp>

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
TwoPartsSource::GenerationInstance::GenerationInstance(
    ConstructInfo constructInfo)
    : SubrangeBase_(std::counted_iterator(
                        Iterator_(*this),
                        static_cast<std::ptrdiff_t>(constructInfo.length)),
                    std::default_sentinel),
      m_(constructInfo.m),
      maxOrd_(constructInfo.maxOrd),
      p_(calcP_(constructInfo.h, maxOrd_, m_)),
      generator_(constructInfo.seed),
      partChoice_(p_) {
}

////////////////////////////////////////////////////////////////////////////////
std::uint64_t TwoPartsSource::GenerationInstance::get_() {
  using UniformDistr = std::uniform_int_distribution<std::uint64_t>;
  auto finalDistr = partChoice_(generator_) ? UniformDistr(0, m_ - 1)
                                            : UniformDistr(m_, maxOrd_ - 1);
  return finalDistr(generator_);
}
