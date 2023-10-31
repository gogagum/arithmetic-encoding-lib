#ifndef SOURCE_ITERATOR_HPP
#define SOURCE_ITERATOR_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>

template <class GenerationInstance>
class SourceIterator {
 public:
  SourceIterator() = default;  // TODO(gogagum): why not to delete?
  SourceIterator(const SourceIterator&) = default;
  SourceIterator(SourceIterator&&) noexcept = default;
  explicit SourceIterator(GenerationInstance& owner);
  SourceIterator& operator=(const SourceIterator<GenerationInstance>&) =
      default;
  SourceIterator& operator=(SourceIterator<GenerationInstance>&&) noexcept =
      default;
  std::uint64_t operator*() const;
  SourceIterator<GenerationInstance>& operator++();
  SourceIterator<GenerationInstance> operator++(int);
  ~SourceIterator() = default;

 private:
  GenerationInstance* owner_;
};

template <class GenerationInstance>
SourceIterator<GenerationInstance>::SourceIterator(GenerationInstance& owner)
    : owner_{&owner} {
}

template <class GenerationInstance>
std::uint64_t SourceIterator<GenerationInstance>::operator*() const {
  return owner_->get_();
}

template <class GenerationInstance>
SourceIterator<GenerationInstance>&
SourceIterator<GenerationInstance>::operator++() {
  return *this;
}

template <class GenerationInstance>
SourceIterator<GenerationInstance>
SourceIterator<GenerationInstance>::operator++(int) {
  return *this;
}

template <class GeneratorInstance>
struct std::iterator_traits<SourceIterator<GeneratorInstance>> {
  using iterator_category = std::input_iterator_tag;
  using value_type = std::uint64_t;
  using reference = std::uint64_t;
  using difference_type = std::ptrdiff_t;
};

#endif
