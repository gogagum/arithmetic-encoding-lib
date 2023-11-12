#ifndef TWO_PARTS_SOURCE_CONDITIONAL_ENTROPY_BASE_HPP
#define TWO_PARTS_SOURCE_CONDITIONAL_ENTROPY_BASE_HPP

#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
/// \brief The SourceConditionalEntropyBase class.
class SourceConditionalEntropyBase {
 public:
  struct ConditionalEntropyRange {
    double minHXX;
    double maxHXX;
  };

 public:
  /**
   * @brief Get minimal conditional entropy reachable with given parameters.
   *
   * @param p - previously chosen p parameter.
   * @param maxOrd - alphabet size.
   * @param m separation parameter.
   * @return minimal conditional entropy.
   */
  static double getMinConditionalEntropy(double p, std::uint64_t maxOrd,
                                         std::uint64_t m);

  /**
   * @brief Get maximal conditional entropy reachable with given parameters.
   *
   * @param p - previously chosen p parameter.
   * @param maxOrd - alphabet size.
   * @param m separation parameter.
   * @return maximal conditional entropy.
   */
  static double getMaxConditionalEntropy(double p, std::uint64_t maxOrd,
                                         std::uint64_t m);

  /**
   * @brief Get minimal and maximal conditional entropies reachable with given
   * parameters.
   *
   * @param p - previously chosen p parameter.
   * @param maxOrd - alphabet size.
   * @param m separation parameter.
   * @return entropy range.
   */
  static ConditionalEntropyRange getMinMaxConditionalEntropy(
      double p, std::uint64_t maxOrd, std::uint64_t m);

  /**
   * @brief Calculate entropy with condition.
   *
   * @param p probability of getting one of first `m` alphabet elements.
   * @param delta conditional probability offset.
   * @param maxOrd alphabet size.
   * @param m separation parameter.
   * @return conditional entropy.
   */
  static double entropyWithCondition(double p, double delta,
                                     std::uint64_t maxOrd, std::uint64_t m);

 protected:
  static double calcDelta_(double hxx, std::uint64_t maxOrd, std::uint64_t m,
                           double p, double h);
};

#endif  // TWO_PARTS_SOURCE_CONDITIONAL_ENTROPY_BASE_HPP
