#ifndef TWO_PARTS_SOURCE_HPP
#define TWO_PARTS_SOURCE_HPP

#include <fmt/format.h>

#include <cstdint>
#include <stdexcept>

template <class OrdT, OrdT maxOrd>
class TwoPartsSource {
 public:
  using Ord = OrdT;

 private:
  class Iterator_{
    explicit Iterator_(TwoPartsSource* ownerPtr) : ownerPtr_{ownerPtr} {};
    
    OrdT operator*() {
      ownerPtr_->get_();
    }

    Iterator_& operator++() {}

    Iterator_& operator++(int) {}

   private:
    TwoPartsSource* ownerPtr_;
  };

 public:
  explicit TwoPartsSource(Ord m, double H) : m_(m), p_(calcP_(H)) {
    if (m_ == 0 || m_ >= maxOrd) {
      throw std::invalid_argument(fmt::format("m = {} has no logic.", m_));
    }
  }

 private:
  double calcP_(double H) {
    if (H < std::log2(m_) || H > std::log2(maxOrd)) {
      throw std::invalid_argument(
          fmt::format("H = {} is not reachable with m = {}, M = {}. "
                      "Minimal H is {}, maximal H is {}",
                      H, m_, maxOrd, enthropy_(0),
                      enthropy_(static_cast<double>(m_) / maxOrd)));
    }
    double pL = 0;
    double pR = static_cast<double>(m_) / maxOrd;
    while (pR - pL > std::numeric_limits<double>::epsilon()) {
      if (const double pM = (pL = pR) / 2; enthropy_(pM) > H) {
        pR = pM;
      } else {
        pL = pM;
      }
    }
    return pL;
  }

  double enthropy_(double p) {
    if (std::abs(p - 1) < std::numeric_limits<double>::epsilon()) {
      return -p * std::log2(p / m_);
    }
    if (p < std::numeric_limits<double>::epsilon()) {
      return -(1 - p) * std::log2((1 - p) / (maxOrd - m_));
    }
    return -p * std::log2(p / m_) -
           (1 - p) * std::log2((1 - p) / (maxOrd - m_));
  }

 private:
  Ord m_;
  double p_;
};

#endif  // TWO_PARTS_SOURCE_HPP
