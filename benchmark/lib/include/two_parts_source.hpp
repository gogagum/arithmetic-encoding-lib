#ifndef TWO_PARTS_SOURCE_HPP
#define TWO_PARTS_SOURCE_HPP

#include <cstdint>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>

#include "impl/source_iterator.hpp"
#include "impl/two_parts_source_entropy_base.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The TwoPartsSource class. Creates a range of generated values with
/// given required parameters and has a set of static methods for entropy
/// calculation.
class TwoPartsSource : public TwoPartsSourceEntropyBase {
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// \brief The GenerationInstance class.
  class GenerationInstance;

 public:
  /**
   * @brief Get lazily generated values as a range.
   *
   * @param maxOrd - alphabet size.
   * @param m - separation parameter.
   * @param h - entropy.
   * @param length - length of the sequence.
   * @param seed - generator seed.
   * @return values as a range.
   */
  static GenerationInstance getGeneration(std::size_t maxOrd, std::size_t m,
                                          double h, std::size_t length,
                                          std::uint64_t seed = 0);
};

////////////////////////////////////////////////////////////////////////////////
class TwoPartsSource::GenerationInstance
    : public std::ranges::subrange<
          std::counted_iterator<SourceIterator<GenerationInstance>>,
          std::default_sentinel_t> {
 private:
  explicit GenerationInstance(std::size_t maxOrd, std::size_t m, double h,
                              std::size_t length, std::uint64_t seed);

 private:
  std::uint64_t get_();

 private:
  using Iterator_ = SourceIterator<GenerationInstance>;
  using SubrangeBase_ = std::ranges::subrange<std::counted_iterator<Iterator_>,
                                              std::default_sentinel_t>;

 private:
  std::uint64_t maxOrd_;
  std::uint64_t m_;
  double p_;
  std::mt19937 generator_;
  std::bernoulli_distribution partChoice_;

 private:
  friend class SourceIterator<GenerationInstance>;
  friend class TwoPartsSource;
};

#endif  // TWO_PARTS_SOURCE_HPP
