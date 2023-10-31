#ifndef SOURCE_ENTROPY_BASE_HPP
#define SOURCE_ENTROPY_BASE_HPP

#include <cstdint>

class SourceEntropyBase {
 public:
  struct HRange {
    double minH;
    double maxH;
  };

 public:
  static double getMinH(std::uint64_t maxOrd, std::uint64_t m);

  static double getMaxH(std::uint64_t maxOrd, std::uint64_t m);

  static HRange getMinMaxH(std::uint64_t maxOrd, std::uint64_t m);

 protected:
  static double entropy_(double p, std::uint64_t maxOrd,
                                       std::uint64_t m);

  static double calcP_(double h, std::uint64_t maxOrd, std::uint64_t m);
};

#endif  // SOURCE_ENTROPY_BASE_HPP
