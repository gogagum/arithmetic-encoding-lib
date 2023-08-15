#include <cstdint>

namespace ael::test {

template <class InternalGen>
class OrdGenerator {
 public:
  OrdGenerator(InternalGen& gen, std::uint64_t maxOrd)
      : gen_(&gen), maxOrd_(maxOrd) {
  }

  std::uint64_t operator()() {
    return gen_->operator()() % maxOrd_;
  }
  
 private:
  InternalGen* gen_;
  std::uint64_t maxOrd_;
};

}  // namespace ael::test
