#include <gtest/gtest.h>

#include <ael/impl/context_buffer.hpp>
#include <cstdint>
#include <deque>
#include <initializer_list>
#include <random>
#include <ranges>

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

TEST(ContextBuffer, Construct) {
  [[maybe_unused]] auto buff = ael::impl::ContextBuffer<std::uint64_t, 3>();
}

TEST(ContextBuffer, OneAdd) {
  auto buff = ael::impl::ContextBuffer<std::uint64_t, 3>{};
  buff.add(42);
}

TEST(ContextBuffer, getEmpty) {
  auto buff = ael::impl::ContextBuffer<std::uint64_t, 3>{};
  auto ctx = buff.getCtx();
  EXPECT_EQ(ctx.size(), 0);
}

TEST(ContextBuffer, getOne) {
  auto buff = ael::impl::ContextBuffer<std::uint64_t, 3>{};
  buff.add(42);
  auto ctx = buff.getCtx();

  EXPECT_EQ(ctx.size(), 1);
}

TEST(ContextBuffer, getFromPartiallyFilledContext) {
  auto buff = ael::impl::ContextBuffer<std::uint64_t, 3>{};
  buff.add(42);
  buff.add(37);

  EXPECT_TRUE(std::ranges::equal(buff.getCtx(), std::array{42, 37}));
}

TEST(ContextBuffer, getFromOnceFilledContext) {
  auto buff = ael::impl::ContextBuffer<std::uint64_t, 3>{};
  for (auto ord: std::array{42, 37, 73}) {
    buff.add(ord);
  }

  EXPECT_TRUE(std::ranges::equal(buff.getCtx(), std::array{42, 37, 73}));
}

TEST(ContextBuffer, getFromFilledMoreThanOnce) {
  auto buff = ael::impl::ContextBuffer<std::uint64_t, 3>{};
  const auto ords = std::array{42, 37, 73, 6, 53};
  for (auto ord : ords) {
    buff.add(ord);
  }

  auto ctx = buff.getCtx();

  EXPECT_TRUE(std::ranges::equal(ctx, std::array{73, 6, 53}));
}

TEST(ContextBuffer, Fuzz) {
  auto buff = ael::impl::ContextBuffer<std::uint64_t, 4>{};
  auto initialElements = std::array{1, 2, 3, 4};

  auto gen = std::mt19937(42);
  for (auto ord: initialElements) {
    buff.add(ord);
  }

  auto ctxReference =
      std::deque<std::uint64_t>(initialElements.begin(), initialElements.end());

  for (auto i : std::views::iota(0, 100)) {
    const auto ord = gen();
    ctxReference.push_back(ord);
    ctxReference.pop_front();
    buff.add(ord);
    EXPECT_TRUE(std::ranges::equal(ctxReference, buff.getCtx()));
  }
}

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
