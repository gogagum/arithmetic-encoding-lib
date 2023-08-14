#ifndef A_D_DICTIONARY_BASE_HPP
#define A_D_DICTIONARY_BASE_HPP

#include <ael/impl/dictionary/cumulative_count.hpp>
#include <ael/impl/dictionary/cumulative_unique_count.hpp>
#include <cstdint>
#include <ranges>
#include "ael/impl/dictionary/max_ord_base.hpp"

namespace ael::impl::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The base A/D dictionary base class.
///
class ADDictionaryBase : protected MaxOrdBase<std::uint64_t> {
 public:
  using Ord = MaxOrdBase::Ord;
  using Count = std::uint64_t;

 public:
  ADDictionaryBase() = delete;

 protected:
  explicit ADDictionaryBase(Ord maxOrd);

  /**
   * @brief get total words count without applying any model.
   * 
   * @return total processed words count.
   */
  [[nodiscard]] Count getRealTotalWordsCnt_() const {
    return cumulativeCnt_.getTotalWordsCnt();
  }

  /**
   * @brief get lower cumulative count without applying any model. 
   * 
   * @param ord index of a word((0, maxOrd]).
   * @return real cumulative count of a word.
   */
  [[nodiscard]] Count getRealLowerCumulativeWordCnt_(Ord ord) const {
    return cumulativeCnt_.getLowerCumulativeCnt(ord);
  }

  /**
   * @brief get real word count without aplying models.
   * 
   * @param ord index of a word.
   * @return real count of a word.
   */
  [[nodiscard]] Count getRealWordCnt_(Ord ord) const {
    return cumulativeCnt_.getCount(ord);
  }

  /**
   * @brief get real total word count.
   * 
   * @return real total words counts.
   */
  [[nodiscard]] Count getTotalWordsUniqueCnt_() const {
    return cumulativeUniqueCnt_.getTotalWordsCnt();
  }

  /**
   * @brief get lower cumulative unique count.
   * 
   * @param ord index of a word.
   * @return lower cumulative unique count.
   */
  [[nodiscard]] Count getLowerCumulativeUniqueNumFound_(Ord ord) const {
    return cumulativeUniqueCnt_.getLowerCumulativeCnt(ord);
  }

  /**
   * @brief get word unique count.
   * 
   * @param ord index of a word.
   * @return unique count of a word (in fact, 0, if it was not found, 1, if
   * was found).
   */
  [[nodiscard]] Count getWordUniqueCnt_(Ord ord) const {
    return cumulativeUniqueCnt_.getCount(ord);
  }

  /**
   * @brief update unique and real counts for a word.
   * 
   * @param ord index of a word.
   * @param cnt count change.
   */
  void updateWordCnt_(Ord ord, Count cnt);

 private:
  CumulativeCount cumulativeCnt_;
  CumulativeUniqueCount cumulativeUniqueCnt_;
};

}  // namespace ael::impl::dict

#endif  // A_D_DICTIONARY_BASE_HPP
