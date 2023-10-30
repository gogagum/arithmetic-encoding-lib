#ifndef TWO_PARTS_SOURCE_WITH_CONDITION_HPP
#define TWO_PARTS_SOURCE_WITH_CONDITION_HPP

#include <cstdint>
#include <random>
#include <optional>

class TwoPartsSourceWithConditions {
 public:
  class GenerationInstance {
   private:
    class Iterator_ {
     public:
      /**
       * @brief Construct a new Iterator_ object
       */
      Iterator_() = default;  // TODO(gogagum): why not to delete?

      Iterator_(const Iterator_& other) = default;

      Iterator_(Iterator_&& other) noexcept = default;

      Iterator_(GenerationInstance& ownerPtr, std::size_t offset);

      Iterator_& operator=(const Iterator_& other) = default;

      Iterator_& operator=(Iterator_&& other) noexcept = default;

      std::uint64_t operator*() const;

      Iterator_& operator++();

      Iterator_ operator++(int);

      bool operator!=(const Iterator_& other) const;

      bool operator==(const Iterator_& other) const;

      ~Iterator_() = default;

     private:
      std::size_t offset_{0};
      GenerationInstance* ownerPtr_;
    };

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

    Iterator_ begin();

    Iterator_ end();

    /**
     * @brief Size of a range.
     *
     * @return std::size_t range size.
     */
    [[nodiscard]] std::size_t size() const;

   private:
    std::uint64_t get_();

   private:
    constexpr static auto half_ = double{0.5};

    std::uint64_t maxOrd_{2};
    std::uint64_t m_{1};
    std::size_t length_{0};
    double p_{half_};
    double delta_{0};
    std::mt19937 generator_{0};  // NOLINT
    std::optional<std::uint64_t> prevGenerated_{};
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

////////////////////////////////////////////////////////////////////////////////
/// \brief iterator_traits for generation iterator.
template <>
struct std::iterator_traits<
    TwoPartsSourceWithConditions::GenerationInstance::Iterator_> {
  using iterator_category = std::input_iterator_tag;
  using value_type = std::uint64_t;
  using reference = std::uint64_t;
  using difference_type = std::ptrdiff_t;
};

#endif  // TWO_PARTS_SOURCE_WITH_CONDITION_HPP
