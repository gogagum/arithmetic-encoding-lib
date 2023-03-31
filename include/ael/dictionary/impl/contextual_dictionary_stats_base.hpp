#ifndef CONTEXTUAL_DICTIONARY_STATS_BASE_HPP
#define CONTEXTUAL_DICTIONARY_STATS_BASE_HPP

#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <unordered_map>

#include "word_probability_stats.hpp"

namespace ael::dict::impl {

namespace bm = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ContextualDictionaryStatsBase<InternalDict> class
///
template <class InternalDict>
class ContextualDictionaryStatsBase : protected InternalDict {
 private:
  constexpr static const std::uint16_t maxCtxBits_ = 56;

 protected:
  using Ord = std::uint64_t;
  using Count = std::uint64_t;
  using Dict_ = InternalDict;

  struct SearchCtx_ {
    std::uint16_t length{};
    Ord ctx{};
    friend bool operator==(SearchCtx_, SearchCtx_) = default;
  };

  struct SearchCtxHash_ {
    std::size_t operator()(SearchCtx_ searchCtx) const {
      return static_cast<std::size_t>(searchCtx.ctx) ^
             std::size_t{searchCtx.length};
    }
  };

  ////////////////////////////////////////////////////////////////////////////
  /// \brief The ContextualDictionaryStatsBase<InternalDict>::ConstructInfo
  /// class
  ///
  struct ConstructInfo {
    std::uint16_t wordNumBits;
    std::uint16_t ctxLength;
    std::uint16_t ctxCellBitsLength;
  };

 public:
  /**
   * @brief contextual dictionary constructor.
   * @param constructInfo - wordBitsCount, context length, context cell bits
   * length.
   */
  explicit ContextualDictionaryStatsBase(ConstructInfo constructInfo);

 protected:
  [[nodiscard]] Ord _getCtx(std::uint16_t length) const;

  SearchCtx_ _getSearchCtx(std::uint16_t length) const;

  void _updateCtx(Ord ord);

  Count _getContextualTotalWordCnt(const SearchCtx_& searchCtx) const;

  Ord _getContextualWordOrd(const SearchCtx_& searchCtx,
                            Count cumulativeCnt) const;

  void _updateContextualDictionary(const SearchCtx_& searchCtx, Ord ord);

  WordProbabilityStats<Count> _getContextualProbStats(
      const SearchCtx_& searchCtx, Ord ord);

  [[nodiscard]] std::uint16_t getCurrCtxLength_() const {
    return currCtxLength_;
  }

  [[nodiscard]] bool ctxExists_(const SearchCtx_& searchCtx) const {
    return contextProbs_.contains(searchCtx);
  }

 private:
  std::unordered_map<SearchCtx_, Dict_, SearchCtxHash_> contextProbs_{};
  const std::uint16_t ctxCellBitsLength_;
  const std::uint16_t ctxLength_;
  const std::uint16_t numBits_;
  Ord ctx_{0};
  std::uint16_t currCtxLength_{0};
};

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
ContextualDictionaryStatsBase<InternalDictT>::ContextualDictionaryStatsBase(
    ConstructInfo constructInfo)
    : ctxCellBitsLength_(constructInfo.ctxCellBitsLength),
      ctxLength_(constructInfo.ctxLength),
      numBits_(constructInfo.wordNumBits),
      InternalDictT(1ull << constructInfo.wordNumBits) {
  if (ctxCellBitsLength_ * ctxLength_ > maxCtxBits_) {
    throw std::invalid_argument("Too big context length.");
  }
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
auto ContextualDictionaryStatsBase<InternalDictT>::_getCtx(
    std::uint16_t len) const -> Ord {
  return ctx_ % (1ull << (ctxCellBitsLength_ * len));
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
auto ContextualDictionaryStatsBase<InternalDictT>::_getSearchCtx(
    std::uint16_t len) const -> SearchCtx_ {
  return {len, _getCtx(len)};
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
void ContextualDictionaryStatsBase<InternalDictT>::_updateCtx(Ord ord) {
  if (currCtxLength_ < ctxLength_) {
    ++currCtxLength_;
  }
  auto newCtx128 = bm::uint128_t{(ctx_ != 0) ? ctx_ - 1 : 0};
  newCtx128 *= (1ull << numBits_);
  newCtx128 += ord;
  newCtx128 %= (1ull << (ctxCellBitsLength_ * ctxLength_));
  ctx_ = newCtx128.convert_to<Ord>();
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
auto ContextualDictionaryStatsBase<InternalDictT>::_getContextualTotalWordCnt(
    const SearchCtx_& searchCtx) const -> Count {
  if (!this->contextProbs_.contains(searchCtx)) {
    return 0;
  }
  return this->contextProbs_.at(searchCtx).getTotalWordsCnt();
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
auto ContextualDictionaryStatsBase<InternalDictT>::_getContextualWordOrd(
    const SearchCtx_& searchCtx, Count cumulativeCnt) const -> Ord {
  return this->contextProbs_.at(searchCtx).getWordOrd(cumulativeCnt);
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
void ContextualDictionaryStatsBase<InternalDictT>::_updateContextualDictionary(
    const SearchCtx_& searchCtx, Ord ord) {
  if (!this->contextProbs_.contains(searchCtx)) {
    this->contextProbs_.emplace(searchCtx, this->getMaxOrd_());
  }
  this->contextProbs_.at(searchCtx)._updateWordCnt(ord, 1);
}

////////////////////////////////////////////////////////////////////////////////
template <class InternalDictT>
auto ContextualDictionaryStatsBase<InternalDictT>::_getContextualProbStats(
    const SearchCtx_& searchCtx, Ord ord) -> WordProbabilityStats<Count> {
  return this->contextProbs_.at(searchCtx).getProbabilityStats(ord);
}

}  // namespace ael::dict::impl

#endif  // CONTEXTUAL_DICTIONARY_STATS_BASE_HPP
