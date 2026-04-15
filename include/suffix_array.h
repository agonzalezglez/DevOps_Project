/**
 * @file suffix_array.h
 * @brief Suffix array definition for exact pattern search.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * A suffix array is a text indexing structure.
 *
 * - all suffixes of a reference string are considered
 * - suffixes are sorted in lexicographic order
 * - this order allows exact pattern search
 *
 * In this project, the suffix array is used to index the reference genome
 * and to search exact occurrences of k-mers extracted from reads.
 */

#ifndef SUFFIX_ARRAY_H
#define SUFFIX_ARRAY_H

#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Suffix array structure for exact substring search.
 *
 * This class stores:
 * - the indexed reference string
 * - the suffix array
 * - the LCP array (Longest Common Prefix array)
 *
 * It provides methods to:
 * - check whether a pattern appears
 * - count pattern occurrences
 * - return all exact occurrence positions
 */
class SuffixArray {
private:
  /**
   * @brief Indexed reference string.
   */
  std::string _ref;

  /**
   * @brief Sorted suffix start positions.
   *
   * Each value in this vector is a starting position in the reference string.
   * The positions are sorted according to the lexicographic order of the
   * corresponding suffixes.
   */
  std::vector<size_t> _sa;

  /**
   * @brief Longest Common Prefix array.
   *
   * LCP[i] stores the common prefix length between:
   * - suffix starting at SA[i]
   * - suffix starting at SA[i+1]
   *
   * The last value is set to 0 by convention.
   */
  std::vector<size_t> _lcp;

  /**
   * @brief Find one occurrence of a query in the suffix array.
   *
   * This method performs a binary search over the sorted suffixes.
   * If one occurrence exists, it returns one matching position in the
   * suffix-array interval.
   *
   * @param query Pattern to search
   * @return Position in the suffix array, or -1 if absent
   */
  size_t _findOccurrence(const std::string &query) const;

  /**
   * @brief Find the first occurrence interval boundary.
   *
   * Once one match is found, the LCP array is used to move left in order
   * to recover the full interval of matching suffixes.
   *
   * @param pos One matching suffix-array position
   * @param len Query length
   * @return First matching position in the suffix array
   */
  size_t _findFirstOccurrence(size_t pos, size_t len) const;

  /**
   * @brief Find the last occurrence interval boundary.
   *
   * Once one match is found, the LCP array is used to move right in order
   * to recover the full interval of matching suffixes.
   *
   * @param pos One matching suffix-array position
   * @param len Query length
   * @return Last matching position in the suffix array
   */
  size_t _findLastOccurrence(size_t pos, size_t len) const;

public:
  /**
   * @brief Construct a suffix array from a reference string.
   *
   * This constructor:
   * - stores the reference string
   * - builds the suffix array
   * - builds the LCP array
   *
   * @param ref Reference string to index
   */
  SuffixArray(const std::string &ref);

  /**
   * @brief Print the full suffix array structure.
   *
   * @param os Output stream
   */
  void print(std::ostream &os = std::cout) const;

  /**
   * @brief Print the suffix starting at a given position.
   *
   * @param pos Start position in the reference
   * @param os Output stream
   */
  void printSuffix(size_t pos, std::ostream &os = std::cout) const;

  /**
   * @brief Check whether a pattern appears in the reference.
   *
   * @param query Pattern to search
   * @return true if the pattern is present
   * @return false otherwise
   */
  bool contains(const std::string &query) const;

  /**
   * @brief Count exact occurrences of a pattern.
   *
   * @param query Pattern to search
   * @return Number of exact occurrences
   */
  size_t nbOccurrences(const std::string &query) const;

  /**
   * @brief Return all exact occurrence positions of a pattern.
   *
   * The returned values are positions in the original reference string.
   *
   * @param query Pattern to search
   * @return Vector of occurrence positions
   */
  std::vector<size_t> occurrences(const std::string &query) const;

  /**
   * @brief Return the indexed reference string.
   *
   * @return Reference string
   */
  const std::string &reference() const;

  /**
   * @brief Return the suffix array.
   *
   * @return Vector of sorted suffix start positions
   */
  const std::vector<size_t> &suffixes() const;
};

inline std::ostream &operator<<(std::ostream &os, const SuffixArray &sa) {
  sa.print(os);
  return os;
}

#endif