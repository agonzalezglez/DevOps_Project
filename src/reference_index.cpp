/**
 * @file reference_index.cpp
 * @brief Implementation of the ReferenceIndex class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This file contains the implementation of the reference index.
 *
 * Steps:
 * - the reference genome is first normalized
 * - a suffix array is built on this normalized reference
 * - later, the mapper uses this index to search exact patterns
 *
 */

#include "reference_index.h"

using namespace std;

/**
 * @brief Normalize one nucleotide in DNA mode.
 *
 * This function simplifies the alphabet used by the project:
 * - lowercase letters are converted to uppercase
 * - U is converted to T
 * - N remains N
 * - any unknown character is converted to N
 *
 * The goal is to keep a consistent internal representation for
 * sequence comparison and pattern search.
 *
 * @param c Input character
 * @return Normalized nucleotide
 */
char ReferenceIndex::normalizeBase(char c) {
  switch (c) {
  case 'a':
  case 'A':
    return 'A';
  case 'c':
  case 'C':
    return 'C';
  case 'g':
  case 'G':
    return 'G';
  case 't':
  case 'T':
    return 'T';
  case 'u':
  case 'U':
    return 'T';
  case 'n':
  case 'N':
    return 'N';
  default:
    return 'N';
  }
}

/**
 * @brief Normalize a full sequence.
 *
 * This method applies normalizeBase() to every character of the input.
 * It is used both for the reference and for searched patterns, so that
 * all exact comparisons are performed on the same alphabet.
 *
 * Example:
 * - "acgu" becomes "ACGT"
 * - unknown symbols become 'N'
 *
 * @param seq Input sequence
 * @return Normalized sequence
 */
string ReferenceIndex::normalizeSequence(const string &seq) {
  string result;
  result.reserve(seq.size());

  for (size_t i = 0; i < seq.size(); ++i) {
    result.push_back(normalizeBase(seq[i]));
  }

  return result;
}

/**
 * @brief Construct a reference index from a sequence.
 *
 * The constructor performs two main tasks:
 * 1. normalize the reference sequence
 * 2. build a suffix array on the normalized reference
 *
 * The value of k is also stored because the mapper uses the same index
 * together with a fixed k-mer size.
 *
 * Important:
 * this class does not extract k-mers by itself.
 * It only stores the indexed reference and supports exact pattern search.
 *
 * @param referenceSequence Input reference genome
 * @param k k-mer size used by the mapper
 */
ReferenceIndex::ReferenceIndex(const string &referenceSequence, int k)
  : _reference(normalizeSequence(referenceSequence)),
    _k(k),
    _suffixArray(_reference) {
}

/**
 * @brief Return all exact positions of a pattern in the reference.
 *
 * The pattern is first normalized so that search is consistent with the
 * normalized reference stored in the index.
 *
 * Then the actual search is delegated to the suffix array.
 *
 * This means that the class acts as a interface between:
 * - the mapper, which asks for pattern positions
 * - the suffix array, which performs the exact search
 *
 * This method can also search any exact pattern of any length.
 *
 * @param pattern Pattern to search in the reference
 * @return List of positions where the pattern appears exactly
 */
vector<int> ReferenceIndex::getPositions(const string &pattern) const {
  vector<int> result;
  string normalized = normalizeSequence(pattern);

  vector<size_t> positions = _suffixArray.occurrences(normalized);
  result.reserve(positions.size());

  for (size_t i = 0; i < positions.size(); ++i) {
    result.push_back(static_cast<int>(positions[i]));
  }

  return result;
}

/**
 * @brief Compute the reverse complement of a pattern.
 *
 * This method is useful because a read or one of its k-mers may appear
 * on the opposite strand of the reference.
 *
 * The sequence is:
 * - normalized
 * - read from right to left
 * - complemented base by base
 *
 * Complement rules used here:
 * - A <-> T
 * - C <-> G
 * - N -> N
 *
 * @param pattern Input pattern
 * @return Reverse complement sequence
 */
string ReferenceIndex::getReverseComplement(const string &pattern) const {
  string normalized = normalizeSequence(pattern);
  string result;
  result.reserve(normalized.size());

  for (string::const_reverse_iterator it = normalized.rbegin(); it != normalized.rend(); ++it) {
    switch (*it) {
    case 'A':
      result.push_back('T');
      break;
    case 'T':
      result.push_back('A');
      break;
    case 'C':
      result.push_back('G');
      break;
    case 'G':
      result.push_back('C');
      break;
    case 'N':
    default:
      result.push_back('N');
      break;
    }
  }

  return result;
}

/**
 * @brief Return positions of a pattern on both strands.
 *
 * This method searches:
 * - the pattern itself on the forward strand
 * - its reverse complement on the opposite strand
 *
 * The returned boolean indicates the strand:
 * - false = forward strand
 * - true  = reverse strand
 *
 * If the reverse complement is identical to the original pattern,
 * it is not searched twice in order to avoid redundant work.
 *
 * This method is useful for situations where the caller wants a strand-aware
 * search directly from the reference index.
 *
 * @param pattern Pattern to search
 * @return List of positions with strand information
 */
vector<pair<int, bool>> ReferenceIndex::getPositionsOnBothStrands(const string &pattern) const {
  vector<pair<int, bool>> result;

  vector<int> direct = getPositions(pattern);
  for (size_t i = 0; i < direct.size(); ++i) {
    result.push_back(make_pair(direct[i], false));
  }

  string reverse = getReverseComplement(pattern);
  if (reverse != normalizeSequence(pattern)) {
    vector<int> opposite = getPositions(reverse);
    for (size_t i = 0; i < opposite.size(); ++i) {
      result.push_back(make_pair(opposite[i], true));
    }
  }

  return result;
}

/**
 * @brief Return the k-mer size associated with this index.
 *
 * The suffix array itself is not limited to this size, but the mapper uses
 * this value to know how reads should be split into k-mers.
 *
 * @return Stored k-mer size
 */
int ReferenceIndex::k() const {
  return _k;
}

/**
 * @brief Return the normalized reference sequence.
 *
 * This gives read-only access to the internal reference stored in the index.
 * It is useful when the mapper needs the reference for full-read verification.
 *
 * @return Indexed reference sequence
 */
const string &ReferenceIndex::reference() const {
  return _reference;
}

/**
 * @brief Return the size of the underlying suffix array structure.
 *
 * This corresponds to the number of indexed suffixes.
 *
 * @return Number of suffixes in the suffix array
 */
size_t ReferenceIndex::size() const {
  return _suffixArray.suffixes().size();
}