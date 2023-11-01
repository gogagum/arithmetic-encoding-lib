#ifndef TWO_PARTS_SOURCE_WITH_CONDITION_HPP
#define TWO_PARTS_SOURCE_WITH_CONDITION_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <ranges>

#include "impl/source_iterator.hpp"
#include "impl/two_parts_source_conditional_entropy_base.hpp"
#include "impl/two_parts_source_entropy_base.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The TwoPartsSourceWithConditions class. Creates a range of generated
/// values with given required parameters and has a set of static methods for
/// entropy and conditional entropy calculation.
class TwoPartsSourceWithConditions : public TwoPartsSourceEntropyBase,
                                     public SourceConditionalEntropyBase {
 private:
  //////////////////////////////////////////////////////////////////////////////
  /// \brief The GenerationInstance class.
  class GenerationInstance;

 public:
  /**
   * @brief Get generated values as a range.
   *
   * @param maxOrd - alphabet size.
   * @param m - separation parameter.
   * @param h - entropy.
   * @param hxx - conditional entropy.
   * @param length - length of the sequence.
   * @param seed - generator seed.
   * @return GenerationInstance
   */
  static GenerationInstance getGeneration(std::size_t maxOrd, std::size_t m,
                                          double h, double hxx,
                                          std::size_t length,
                                          std::uint64_t seed = 0);
};

////////////////////////////////////////////////////////////////////////////////
class TwoPartsSourceWithConditions::GenerationInstance
    : public std::ranges::subrange<
          std::counted_iterator<SourceIterator<GenerationInstance>>,
          std::default_sentinel_t> {
 private:
  GenerationInstance(std::size_t maxOrd, std::size_t m, double h, double hxx,
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
  double delta_;
  std::mt19937 generator_;
  std::optional<std::uint64_t> prevGenerated_{};

 private:
  friend class SourceIterator<GenerationInstance>;
  friend class TwoPartsSourceWithConditions;
};

#endif  // TWO_PARTS_SOURCE_WITH_CONDITION_HPP
