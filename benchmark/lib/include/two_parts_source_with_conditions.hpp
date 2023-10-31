#ifndef TWO_PARTS_SOURCE_WITH_CONDITION_HPP
#define TWO_PARTS_SOURCE_WITH_CONDITION_HPP

#include <cstddef>
#include <cstdint>
#include <impl/source_iterator.hpp>
#include <optional>
#include <random>
#include <ranges>

class TwoPartsSourceWithConditions {
 public:
  class GenerationInstance
      : public std::ranges::subrange<
            std::counted_iterator<SourceIterator<GenerationInstance>>,
            std::default_sentinel_t> {
   public:
    struct ConstructInfo {
      std::uint64_t maxOrd{2};
      std::uint64_t m{1};
      double h{1};
      double hxx{1};
      std::size_t length{0};
      std::uint64_t seed{0};
    };

    struct HRange {
      double min;
      double max;
    };

   public:
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
    double delta_{0};
    std::mt19937 generator_{0};  // NOLINT
    std::optional<std::uint64_t> prevGenerated_{};

   private:
    friend class SourceIterator<GenerationInstance>;
  };

 public:
  struct GenerationConfig {
    std::size_t maxOrd{2};
    std::size_t m{1};
    double h{1};
    double hxx{1};
    std::size_t length{0};
    std::uint64_t seed{0};
  };

 public:
  static double getMinH(std::uint64_t maxOrd, std::uint64_t m);

  static double getMaxH(std::uint64_t maxOrd, std::uint64_t m);

  static GenerationInstance::HRange getMinMaxH(std::uint64_t maxOrd,
                                               std::uint64_t m);

  static double getMinHXX(std::uint64_t maxOrd, std::uint64_t m);
  static double getMaxHXX(std::uint64_t maxOrd, std::uint64_t m);

  static GenerationInstance::HRange getMinMaxHXX(std::uint64_t maxOrd,
                                                 std::uint64_t m);

  static GenerationInstance getGeneration(GenerationConfig GenerationConfig);

 private:
  static double calcP_(double h, std::uint64_t maxOrd, std::uint64_t m);

  static double calcDelta_(double hxx, std::uint64_t maxOrd, std::uint64_t m,
                           double p, double h);

  [[nodiscard]] static double entropy_(double p, std::uint64_t maxOrd,
                                       std::uint64_t m);

  [[nodiscard]] static double entropyWithCondition_(double p, double delta,
                                                    std::uint64_t maxOrd,
                                                    std::uint64_t m);
};

#endif  // TWO_PARTS_SOURCE_WITH_CONDITION_HPP
