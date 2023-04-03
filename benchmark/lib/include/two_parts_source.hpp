#ifndef TWO_PARTS_SOURCE_HPP
#define TWO_PARTS_SOURCE_HPP

#include <bits/ranges_base.h>
#include <fmt/format.h>

#include <cstdint>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>

class TwoPartsSource {
 public:
  class GenerationInstance {
   private:
    class Iterator_ : std::input_iterator_tag {
     public:
      Iterator_(GenerationInstance* ownerPtr, std::size_t offset);

      /**
       * @brief get generated element of a sequence.
       *
       * @return OrdT element of a sequence.
       */
      std::uint64_t operator*();

      /**
       * @brief increment of an iterator.
       *
       * @return Iterator_& referense to self.
       */
      Iterator_& operator++();

      bool operator!=(const Iterator_& other) const {
        return offset_ < other.offset_ || ownerPtr_ != other.ownerPtr_;
      }

      bool operator==(const Iterator_& other) const {
        return offset_ < other.offset_ && ownerPtr_ == other.ownerPtr_;
      }

     private:
      std::size_t offset_;
      GenerationInstance* ownerPtr_;
    };

   public:
    /**
     * @brief Construct a new Generation Instance object
     *
     * @param count number of elements in the sequence.
     */
    GenerationInstance(std::uint64_t m, std::uint64_t maxOrd, double h,
                       std::size_t length, std::uint64_t seed);

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
    std::uint64_t get_() {
      return 0;  // TODO
    }

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
  static GenerationInstance getGeneration(std::size_t maxOrd, std::size_t m,
                                          double h, std::size_t length,
                                          std::uint64_t seed = 0);
};

template <>
struct std::iterator_traits<TwoPartsSource::GenerationInstance::Iterator_> {
  using iterator_category = std::input_iterator_tag;
};

////////////////////////////////////////////////////////////////////////////////
TwoPartsSource::GenerationInstance::Iterator_::Iterator_(
    GenerationInstance* ownerPtr, std::size_t offset)
    : offset_{offset}, ownerPtr_{ownerPtr} {};

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::GenerationInstance::Iterator_::operator*()
    -> std::uint64_t {
  return ownerPtr_->get_();
}

////////////////////////////////////////////////////////////////////////////////
auto TwoPartsSource::GenerationInstance::Iterator_::operator++() -> Iterator_& {
  ++offset_;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
TwoPartsSource::GenerationInstance::GenerationInstance(std::uint64_t m,
                                                       std::uint64_t maxOrd,
                                                       double h,
                                                       std::size_t length,
                                                       std::uint64_t seed)
    : m_(m), maxOrd_(maxOrd), length_(length), p_(calcP_(h)), generator_(seed) {
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
auto TwoPartsSource::getGeneration(std::uint64_t maxOrd, std::uint64_t m,
                                   double h, std::size_t length,
                                   std::uint64_t seed) -> GenerationInstance {
  if (m == 0 || m >= maxOrd) {
    throw std::invalid_argument(fmt::format("m = {} has no logic.", m));
  }
  return {m, maxOrd, h, length, seed};
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
  double pR = static_cast<double>(m_) / static_cast<double>(maxOrd_);
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
