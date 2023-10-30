#ifndef TWO_PARTS_SOURCE_HPP
#define TWO_PARTS_SOURCE_HPP

#include <cstdint>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>

class TwoPartsSource {
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// \brief The GenerationInstance class.
  ///
  class GenerationInstance {
   private:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief The GenerationInstance::Iterator_ class.
    ///
    class Iterator_ {
     public:
      /**
       * @brief Construct a new Iterator_ object
       */
      Iterator_() = default;  // TODO(gogagum): why not to delete?

      /**
       * @brief Construct a new Iterator_ object from copy.
       *
       * @param other iterator to copy from.
       */
      Iterator_(const Iterator_& other) = default;

      /**
       * @brief Construct a new Iterator_ object moving.
       *
       * @param other iterator to move from.
       */
      Iterator_(Iterator_&& other) = default;

      /**
       * @brief Construct a new Iterator_ object
       *
       * @param ownerPtr pointer to instance.
       * @param offset offset of the iterator.
       */
      Iterator_(GenerationInstance& ownerPtr, std::size_t offset);

      /**
       * @brief Copy assign operator.
       *
       * @param other iterator to copy from.
       * @return Iterator_& reference to itself.
       */
      Iterator_& operator=(const Iterator_& other) = default;

      /**
       * @brief Move assign operator.
       *
       * @param other iterator to move from.
       * @return Iterator_& reference to itself.
       */
      Iterator_& operator=(Iterator_&& other) noexcept = default;

      /**
       * @brief get generated element of a sequence.
       *
       * @return OrdT element of a sequence.
       */
      std::uint64_t operator*() const;

      /**
       * @brief prefix increment of an iterator.
       *
       * @return Iterator_& referense to self.
       */
      Iterator_& operator++();

      /**
       * @brief postfix increment of an iterator.
       *
       * @return Iterator_ copy of self before incrementing.
       */
      Iterator_ operator++(int);

      /**
       * @brief Non-equality with other iterator.
       *
       * @param other iterator
       * @return true if iterators are not equal.
       * @return false if iterators are equal.
       */
      bool operator!=(const Iterator_& other) const;

      /**
       * @brief Equality with other iterator.
       *
       * @param other iterator.
       * @return true if iterators are equal.
       * @return false if iterators are not equal.
       */
      bool operator==(const Iterator_& other) const;

      /**
       * @brief Destroy the Iterator_ object
       */
      ~Iterator_() = default;

     private:
      std::size_t offset_{0};
      GenerationInstance* ownerPtr_{nullptr};
    };

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
    std::mt19937 generator_{0};  // NOLINT
  };

 public:
  struct GenerationConfig {
    std::size_t maxOrd{2};
    std::size_t m{1};
    double h{1};
    std::size_t length{0};
    std::uint64_t seed{0};
  };

 public:
  static double getMinH(std::uint64_t maxOrd, std::uint64_t m);

  static double getMaxH(std::uint64_t maxOrd, std::uint64_t m);

  static GenerationInstance::HRange getMinMaxH(std::uint64_t maxOrd,
                                               std::uint64_t m);

  static GenerationInstance getGeneration(GenerationConfig generationConfig);

 private:
  static double calcP_(double h, std::uint64_t maxOrd, std::uint64_t m);

  [[nodiscard]] static double entropy_(double p, std::uint64_t maxOrd,
                                        std::uint64_t m);
};

////////////////////////////////////////////////////////////////////////////////
/// \brief iterator_traits for generation iterator.
template <>
struct std::iterator_traits<TwoPartsSource::GenerationInstance::Iterator_> {
  using iterator_category = std::input_iterator_tag;
  using value_type = std::uint64_t;
  using reference = std::uint64_t;
  using difference_type = std::ptrdiff_t;
};

#endif  // TWO_PARTS_SOURCE_HPP
