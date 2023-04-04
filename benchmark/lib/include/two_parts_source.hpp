#ifndef TWO_PARTS_SOURCE_HPP
#define TWO_PARTS_SOURCE_HPP

#include <fmt/format.h>

#include <cstdint>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>

class TwoPartsSource {
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// \brief The GenerationInstance class.
  ///
  class GenerationInstance {
   private:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief The GenerationInstance::Iterator_ class.
    ///
    class Iterator_ : std::input_iterator_tag {
     public:
      /**
       * @brief Construct a new Iterator_ object
       */
      Iterator_() = default;

      /**
       * @brief Construct a new Iterator_ object from copy.
       * 
       * @param other iterator to copy from.
       */
      Iterator_(const Iterator_& other) = default;

      /**
       * @brief Construct a new Iterator_ object moving.
       * 
       * @param other iterator to move from.
       */
      Iterator_(Iterator_&& other) = default;

      /**
       * @brief Construct a new Iterator_ object
       *
       * @param ownerPtr pointer to instance.
       * @param offset offset of the iterator.
       */
      Iterator_(GenerationInstance* ownerPtr, std::size_t offset);

      /**
       * @brief Copy assign operator.
       * 
       * @param other iterator to copy from.
       * @return Iterator_& reference to itself.
       */
      Iterator_& operator=(const Iterator_& other) = default;

      /**
       * @brief Move assign operator.
       * 
       * @param other iterator to move from.
       * @return Iterator_& reference to itself.
       */
      Iterator_& operator=(Iterator_&& other) noexcept = default;

      /**
       * @brief get generated element of a sequence.
       *
       * @return OrdT element of a sequence.
       */
      std::uint64_t operator*() const;

      /**
       * @brief prefix increment of an iterator.
       *
       * @return Iterator_& referense to self.
       */
      Iterator_& operator++();

      /**
       * @brief postfix increment of an iterator.
       *
       * @return Iterator_ copy of self before incrementing.
       */
      Iterator_ operator++(int);

      /**
       * @brief Non-equality with other iterator.
       *
       * @param other iterator
       * @return true if iterators are not equal.
       * @return false if iterators are equal.
       */
      bool operator!=(const Iterator_& other) const;

      /**
       * @brief Equality with other iterator.
       *
       * @param other iterator.
       * @return true if iterators are equal.
       * @return false if iterators are not equal.
       */
      bool operator==(const Iterator_& other) const;

      /**
       * @brief Destroy the Iterator_ object
       */
      ~Iterator_() = default;

     private:
      std::size_t offset_{0};
      GenerationInstance* ownerPtr_{nullptr};
    };

   public:
    struct ConstructInfo {
      std::uint64_t m{0};
      std::uint64_t maxOrd{2};
      double h{1};
      std::size_t length{0};
      std::uint64_t seed{0};
    };

   public:
    /**
     * @brief Construct a new Generation Instance object
     *
     * @param count number of elements in the sequence.
     */
    explicit GenerationInstance(ConstructInfo constructInfo);

    /**
     * @brief Generation beginning.
     *
     * @return Iterator_ to the beginning of generation.
     */
    Iterator_ begin();

    /**
     * @brief Generation ending.
     *
     * @return Iterator_ to the ending of generation.
     */
    Iterator_ end();

   private:
    std::uint64_t get_();

    double calcP_(double h);

    [[nodiscard]] double enthropy_(double p) const;

   private:
    std::uint64_t maxOrd_;
    std::uint64_t m_;
    std::size_t length_;
    double p_;
    std::mt19937 generator_;
  };

 public:
  struct GenerationConfig {
    std::size_t maxOrd{2};
    std::size_t m{1};
    double h{1};
    std::size_t length{0};
    std::uint64_t seed{0};
  };

  static GenerationInstance getGeneration(GenerationConfig generationConfig);
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The
/// std::iterator_traits<TwoPartsSource::GenerationInstance::Iterator_>
template <>
struct std::iterator_traits<TwoPartsSource::GenerationInstance::Iterator_> {
  using iterator_category = std::input_iterator_tag;
  using value_type = std::uint64_t;
  using reference = std::uint64_t;
  using difference_type = std::ptrdiff_t;
};

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
TwoPartsSource::GenerationInstance::GenerationInstance(
    ConstructInfo constructInfo)
    : m_(constructInfo.m),
      maxOrd_(constructInfo.maxOrd),
      length_(constructInfo.length),
      p_(calcP_(constructInfo.h)),
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
std::uint64_t TwoPartsSource::GenerationInstance::get_() {
  constexpr auto ticks = std::uint64_t{1 << 16};
  const auto part = std::uint64_t{generator_() % ticks};
  if (static_cast<double>(part) / static_cast<double>(ticks) < p_) {
    return generator_() % m_;
  }
  return m_ + generator_() % (maxOrd_ - m_);
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::getGeneration(GenerationConfig generationConfig)
    -> GenerationInstance {
  if (generationConfig.m == 0 ||
      generationConfig.m >= generationConfig.maxOrd) {
    throw std::invalid_argument(
        fmt::format("m = {} has no logic.", generationConfig.m));
  }
  return GenerationInstance({generationConfig.m, generationConfig.maxOrd,
                             generationConfig.h, generationConfig.length,
                             generationConfig.seed});
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSource::GenerationInstance::calcP_(double h) {
  if (h < std::log2(m_) || h > std::log2(maxOrd_)) {
    throw std::invalid_argument(fmt::format(
        "H = {} is not reachable with m = {}, M = {}. "
        "Minimal H is {}, maximal H is {}",
        h, m_, maxOrd_, enthropy_(0),
        enthropy_(static_cast<double>(m_) / static_cast<double>(maxOrd_))));
  }
  double pL = 0;
  auto pR = static_cast<double>(m_) / static_cast<double>(maxOrd_);
  while (pR - pL > std::numeric_limits<double>::epsilon()) {
    if (const double pM = (pL + pR) / 2; enthropy_(pM) > h) {
      pR = pM;
    } else {
      pL = pM;
    }
  }
  return pL;
}

////////////////////////////////////////////////////////////////////////////////
double TwoPartsSource::GenerationInstance::enthropy_(double p) const {
  const auto mDouble = static_cast<double>(m_);
  if (std::abs(p - 1) < std::numeric_limits<double>::epsilon()) {
    return -p * std::log2(p / mDouble);
  }
  if (p < std::numeric_limits<double>::epsilon()) {
    return -(1 - p) *
           std::log2((1 - p) / (static_cast<double>(maxOrd_) - mDouble));
  }
  return -p * std::log2(p / static_cast<double>(m_)) -
         (1 - p) *
             std::log2((1 - p) / (static_cast<double>(maxOrd_) - mDouble));
}

#endif  // TWO_PARTS_SOURCE_HPP
