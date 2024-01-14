#include <benchmark/benchmark.h>

#include <ael/arithmetic_coder.hpp>
#include <ael/dictionary/adaptive_a_contextual_dictionary.hpp>
#include <ael/dictionary/adaptive_a_contextual_dictionary_improved.hpp>
#include <ael/dictionary/adaptive_a_dictionary.hpp>
#include <ael/dictionary/adaptive_d_contextual_dictionary.hpp>
#include <ael/dictionary/adaptive_d_contextual_dictionary_improved.hpp>
#include <ael/dictionary/adaptive_d_dictionary.hpp>
#include <ael/dictionary/adaptive_dictionary.hpp>
#include <ael/dictionary/ppma_dictionary.hpp>
#include <ael/dictionary/ppmd_dictionary.hpp>
#include <ael/esc/arithmetic_coder.hpp>
#include <ael/esc/dictionary/adaptive_a_dictionary.hpp>
#include <ael/esc/dictionary/adaptive_d_dictionary.hpp>
#include <ael/esc/dictionary/ppma_dictionary.hpp>
#include <ael/esc/dictionary/ppmd_dictionary.hpp>
#include <two_parts_source.hpp>

using std::views::iota;

static constexpr auto maxOrd = std::uint64_t{256};
static constexpr auto m = std::uint64_t{64};

static void setInputSizesAndMParameters(benchmark::internal::Benchmark* b) {
  for (std::size_t i : iota(1, 11)) {
    for (std::size_t j : iota(0, 5)) {
      const auto inputSize = static_cast<std::int64_t>((1ull << 17) * i / 11);
      const auto hQuarter = static_cast<std::int64_t>(j);  // 0, 1, 2, 3, 4
      b->Args({inputSize, hQuarter});
    }
  }
}

template <class DictInitializer>
static void runTests(benchmark::State& state, DictInitializer dictInitializer,
                     std::uint64_t maxOrd, std::uint64_t m,
                     std::size_t seqLength, std::uint8_t hQuarter) {
  const auto [minH, maxH] = TwoPartsSource::getMinMaxEntropy(maxOrd, m);
  const auto h = minH + (maxH - minH) * hQuarter / 4;

  constexpr auto seed = std::uint64_t{42};

  auto src = TwoPartsSource::getGeneration(maxOrd, m, h, seqLength, seed);

  for ([[maybe_unused]] const auto st : state) {
    auto dict = dictInitializer();
    auto dataConstructor = ael::ByteDataConstructor();

    [[maybe_unused]] auto encoded =
        ael::ArithmeticCoder().encode(src, dict).finalize();
    state.PauseTiming();
    state.counters["encoded_size"] = encoded.bitsEncoded;
    state.ResumeTiming();
  }
}

template <class DictInitializer>
static void runEscTests(benchmark::State& state,
                        DictInitializer dictInitializer, std::uint64_t maxOrd,
                        std::uint64_t m, std::size_t seqLength,
                        std::uint8_t hQuarter) {
  const auto [minH, maxH] = TwoPartsSource::getMinMaxEntropy(maxOrd, m);
  const auto h = minH + (maxH - minH) * hQuarter / 4;

  constexpr auto seed = std::uint64_t{42};

  auto src = TwoPartsSource::getGeneration(maxOrd, m, h, seqLength, seed);

  for ([[maybe_unused]] const auto st : state) {
    auto dict = dictInitializer();
    auto dataConstructor = ael::ByteDataConstructor();

    [[maybe_unused]] auto encoded =
        ael::esc::ArithmeticCoder().encode(src, dict);
  }
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_adaptive_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto ratio = std::size_t{10};

  const auto dictInitializer = [ratio]() {
    return ael::dict::AdaptiveDictionary({maxOrd, ratio});
  };

  runTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_adaptive_a_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  const auto dictInitializer = []() {
    return ael::dict::AdaptiveADictionary(maxOrd);
  };

  runTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_adaptive_d_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  const auto dictInitializer = []() {
    return ael::dict::AdaptiveDDictionary(maxOrd);
  };

  runTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_adaptive_a_contextual_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto dictNumBits = std::uint8_t{8};

  const auto dictInitializer = []() {
    return ael::dict::AdaptiveAContextualDictionary({dictNumBits, 4, 4});
  };

  runTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_adaptive_d_contextual_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto dictNumBits = std::uint8_t{8};

  const auto dictInitializer = []() {
    return ael::dict::AdaptiveDContextualDictionary({dictNumBits, 4, 4});
  };

  runTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_adaptive_a_contextual_improved_coder(
    benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto dictNumBits = std::uint8_t{8};

  const auto dictInitializer = []() {
    return ael::dict::AdaptiveAContextualDictionaryImproved(
        {dictNumBits, 4, 4});
  };

  runTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_adaptive_d_contextual_improved_coder(
    benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto dictNumBits = std::uint8_t{8};

  const auto dictInitializer = []() {
    return ael::dict::AdaptiveDContextualDictionaryImproved(
        {dictNumBits, 4, 4});
  };

  runTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_ppma_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto ctxLength = std::uint8_t{2};

  const auto dictInitializer = []() {
    return ael::dict::PPMADictionary({maxOrd, ctxLength});
  };

  runTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_ppmd_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto ctxLength = std::uint8_t{2};

  const auto dictInitializer = []() {
    return ael::dict::PPMDDictionary({maxOrd, ctxLength});
  };

  runTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_esc_adaptive_a_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto ctxLength = std::uint8_t{2};

  const auto dictInitializer = []() {
    return ael::esc::dict::AdaptiveADictionary(maxOrd);
  };

  runEscTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_esc_adaptive_d_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto ctxLength = std::uint8_t{2};

  const auto dictInitializer = []() {
    return ael::esc::dict::AdaptiveDDictionary(maxOrd);
  };

  runEscTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_esc_ppma_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto ctxLength = std::uint8_t{2};

  const auto dictInitializer = []() {
    return ael::esc::dict::PPMADictionary({maxOrd, ctxLength});
  };

  runEscTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

////////////////////////////////////////////////////////////////////////////////
static void BM_benchmark_esc_ppmd_coder(benchmark::State& state) {
  const auto seqLength = static_cast<std::size_t>(state.range(0));
  const auto hQuarter = static_cast<std::uint8_t>(state.range(1));

  constexpr auto ctxLength = std::uint8_t{2};

  const auto dictInitializer = []() {
    return ael::esc::dict::PPMDDictionary({maxOrd, ctxLength});
  };

  runEscTests(state, dictInitializer, maxOrd, m, seqLength, hQuarter);
}

// NOLINTBEGIN(modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays,
// cppcoreguidelines-owning-memory)

BENCHMARK(BM_benchmark_adaptive_coder)->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_adaptive_a_coder)->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_adaptive_d_coder)->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_adaptive_a_contextual_coder)
    ->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_adaptive_d_contextual_coder)
    ->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_adaptive_a_contextual_improved_coder)
    ->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_adaptive_d_contextual_improved_coder)
    ->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_ppma_coder)->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_ppmd_coder)->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_esc_adaptive_a_coder)
    ->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_esc_adaptive_d_coder)
    ->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_esc_ppma_coder)->Apply(setInputSizesAndMParameters);
BENCHMARK(BM_benchmark_esc_ppmd_coder)->Apply(setInputSizesAndMParameters);

BENCHMARK_MAIN();

// NOLINTEND(modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays,
// cppcoreguidelines-owning-memory)
