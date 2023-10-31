#ifndef TWO_PARTS_SOURCE_HPP
#define TWO_PARTS_SOURCE_HPP

#include <cstdint>
#include <impl/source_entropy_base.hpp>
#include <impl/source_iterator.hpp>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>

class TwoPartsSource : public SourceEntropyBase {
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// \brief The GenerationInstance class.
  ///
  class GenerationInstance
      : public std::ranges::subrange<
            std::counted_iterator<SourceIterator<GenerationInstance>>,
            std::default_sentinel_t> {
   public:
    struct ConstructInfo {
      std::uint64_t maxOrd{2};
      std::uint64_t m{1};
      double h{1};
      std::size_t length{0};
      std::uint64_t seed{0};
    };

    struct HRange {
      double min;
      double max;
    };

   public:
    /**
     * @brief Construct a new Generation Instance object
     *
     * @param count number of elements in the sequence.
     */
    explicit GenerationInstance(ConstructInfo constructInfo);

   private:
    std::uint64_t get_();

   private:
    using Iterator_ = SourceIterator<GenerationInstance>;
    using SubrangeBase_ =
        std::ranges::subrange<std::counted_iterator<Iterator_>,
                              std::default_sentinel_t>;

   private:
    constexpr static auto half_ = double{0.5};

    std::uint64_t maxOrd_{2};
    std::uint64_t m_{1};
    double p_{half_};
    std::mt19937 generator_{0};  // NOLINT
    std::bernoulli_distribution partChoice_;

   private:
    friend class SourceIterator<GenerationInstance>;
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

#endif  // TWO_PARTS_SOURCE_HPP
