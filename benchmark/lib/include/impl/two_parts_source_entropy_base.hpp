#ifndef TWO_PARTS_SOURCE_ENTROPY_BASE_HPP
#define TWO_PARTS_SOURCE_ENTROPY_BASE_HPP

#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
/// \brief The TwoPartsSourceEntropyBase class.
class TwoPartsSourceEntropyBase {
 public:
  struct EntropyRange {
    double minH;
    double maxH;
  };

 public:

  /**
   * @brief Get minimal entropy reachable with `maxOrd` alphabet size and `m`
   * parameter.
   * 
   * @param maxOrd alphabet size.
   * @param m separation parameter.
   * @return minimal entropy.
   */
  static double getMinEntropy(std::uint64_t maxOrd, std::uint64_t m);

  /**
   * @brief Get minimal entropy reachable with `maxOrd` alphabet size and `m`
   * parameter.
   * 
   * @param maxOrd alphabet size.
   * @param m separation parameter.
   * @return maximal entropy.
   */
  static double getMaxEntropy(std::uint64_t maxOrd, std::uint64_t m);

  /**
   * @brief Get minimal and maximal entropy reachable with `maxOrd` alphabet
   * size and `m` parameter.
   * 
   * @param maxOrd alphabet size.
   * @param m separation parameter.
   * @return entropy range.
   */
  static EntropyRange getMinMaxEntropy(std::uint64_t maxOrd, std::uint64_t m);

  /**
   * @brief Calculate entropy with given parameters.
   * 
   * @param p probability of getting one of first `m` alphabet elements.
   * @param maxOrd alphabet size.
   * @param m separation parameter.
   * @return entropy.
   */
  static double entropy(double p, std::uint64_t maxOrd, std::uint64_t m);

 protected:
  static double calcP_(double h, std::uint64_t maxOrd, std::uint64_t m);
};

#endif  // TWO_PARTS_SOURCE_ENTROPY_BASE_HPP
