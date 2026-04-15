/**
 * @file reference_index.h
 * @brief Reference genome indexing using a suffix array.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This class builds an index over a reference sequence to allow efficient
 * substring searches.
 */

#ifndef REFERENCE_INDEX_H
#define REFERENCE_INDEX_H

#include <string>
#include <vector>

#include "suffix_array.h"

/**
 * @brief Index structure for a reference genome.
 *
 * This class wraps a suffix array built on the reference sequence.
 * It provides an interface for retrieving positions of k-mers used
 * during the mapping process.
 */
class ReferenceIndex {
private:
  /**
   * @brief Normalized reference sequence.
   */
  std::string _reference;

  /**
   * @brief Size of k-mers used by the mapper.
   */
  int _k;

  /**
   * @brief Suffix array built on the reference.
   */
  SuffixArray _suffixArray;

  static char normalizeBase(char c);
  static std::string normalizeSequence(const std::string &seq);
  std::string getReverseComplement(const std::string &kmer) const;

public:
  /**
   * @brief Construct the index from a reference sequence.
   *
   * @param referenceSequence Input genome sequence
   * @param k Size of k-mers used during mapping
   */
  ReferenceIndex(const std::string &referenceSequence, int k);

  /**
   * @brief Get positions of a k-mer in the reference.
   *
   * @param kmer Query substring
   * @return Vector of positions
   */
  std::vector<int> getPositions(const std::string &kmer) const;

  /**
   * @brief Get positions on both strands.
   *
   * @param kmer Query substring
   * @return Vector of (position, isReverseStrand)
   */
  std::vector<std::pair<int, bool>> getPositionsOnBothStrands(const std::string &kmer) const;

  /**
   * @brief Get k-mer size.
   */
  int k() const;

  /**
   * @brief Get reference sequence.
   */
  const std::string &reference() const;

  /**
   * @brief Get number of indexed suffixes.
   */
  size_t size() const;
};

#endif