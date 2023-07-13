#include <benchmark/benchmark.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/dictionary/adaptive_dictionary.hpp>
#include <iostream>
#include <two_parts_source.hpp>

static void BM_benchmark_adaptive_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));

  const auto alphabetSize = std::size_t{256};
  const auto m = std::size_t{64};
  const auto h = 7.75;
  const std::uint64_t seed = 42;

  auto generationParams =
      TwoPartsSource::GenerationConfig{alphabetSize, m, h, seqLength, seed};

  auto src = TwoPartsSource::getGeneration(generationParams);

  const std::size_t ratio = 10;

  for (auto st : state) {
    auto dict = ael::dict::AdaptiveDictionary({alphabetSize, ratio});
    auto dataConstructor = ael::ByteDataConstructor();

    [[maybe_unused]] auto encoded =
        ael::ArithmeticCoder::encode(src, dataConstructor, dict);
  }
}

// NOLINTBEGIN(modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays,
// cppcoreguidelines-owning-memory)

BENCHMARK(BM_benchmark_adaptive_coder)->RangeMultiplier(10)->Range(10, 100000);

BENCHMARK_MAIN();

// NOLINTEND(modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays,
// cppcoreguidelines-owning-memory)