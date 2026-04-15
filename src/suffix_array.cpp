/**
 * @file suffix_array.cpp
 * @brief Implementation of the SuffixArray class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This file implements the suffix array used to index the reference genome.
 *
 * Steps:
 * - all suffixes of the reference are represented by their start positions
 * - these positions are sorted lexicographically
 * - exact pattern search is then performed by binary search
 *
 * In addition, an LCP array is built to recover the full interval of matches
 * once one occurrence of the pattern has been found.
 */

#include "suffix_array.h"

#include <algorithm>

using namespace std;

/**
 * @brief Compare two substrings starting at given positions.
 *
 * This helper function compares:
 * - s1 starting at p1
 * - s2 starting at p2
 *
 * It returns:
 * - a negative value if the first substring is lexicographically smaller
 * - a positive value if the first substring is lexicographically larger
 * - zero if both compared parts are equal until one ends
 *
 * The absolute value also contains information about the common prefix length.
 * This is useful when computing LCP values and during search.
 *
 * @param s1 First string
 * @param p1 Start position in first string
 * @param s2 Second string
 * @param p2 Start position in second string
 * @return Comparison result with prefix-length information
 */
static int _compareStrings(const string &s1, size_t p1,
                           const string &s2, size_t p2) {
  int cmp = 0;
  int lcp = 1;
  size_t n1 = s1.length();
  size_t n2 = s2.length();

  while (!cmp && (p1 < n1) && (p2 < n2)) {
    if (s1[p1] < s2[p2]) {
      cmp = -lcp;
    } else if (s1[p1] > s2[p2]) {
      cmp = lcp;
    } else {
      ++p1;
      ++p2;
      ++lcp;
    }
  }

  if (!cmp) {
    if (p1 < n1) {
      cmp = lcp;
    } else if (p2 < n2) {
      cmp = -lcp;
    }
  }

  return cmp;
}

/**
 * @brief Comparator used to sort suffix positions.
 *
 * The suffix array does not store suffix strings directly.
 * It only stores their starting positions in the reference string.
 * This comparator allows sorting these positions according to the
 * lexicographic order of the corresponding suffixes.
 */
struct SuffixComparator {
  /**
   * @brief Indexed reference string.
   */
  const string &ref;

  /**
   * @brief Construct comparator from a reference string.
   *
   * @param r Reference string
   */
  SuffixComparator(const string &r) : ref(r) {
  }

  /**
   * @brief Compare two suffix start positions.
   *
   * @param i First suffix start position
   * @param j Second suffix start position
   * @return true if suffix i is lexicographically smaller than suffix j
   */
  bool operator()(size_t i, size_t j) const {
    return _compareStrings(ref, i, ref, j) < 0;
  }
};

/**
 * @brief Find one occurrence of a query by binary search.
 *
 * This method searches the ordered suffix array for one suffix that
 * starts with the query.
 *
 * If a match is found, the returned position is not necessarily the first
 * or the last one. The full interval is later recovered with the LCP array.
 *
 * @param query Pattern to search
 * @return One matching suffix-array position, or -1 if absent
 */
size_t SuffixArray::_findOccurrence(const string &query) const {
  size_t res = static_cast<size_t>(-1);
  size_t start = 0;
  size_t end = _sa.size();
  size_t m = query.length();

  while ((res == static_cast<size_t>(-1)) && (start < end)) {
    size_t p = (start + end) / 2;
    int cmp = _compareStrings(_ref, _sa[p], query, 0);
    size_t len = (cmp < 0 ? -cmp : cmp) - 1;

    if (!cmp || (len >= m)) {
      res = p;
    } else if (cmp < 0) {
      start = p + 1;
    } else {
      end = p;
    }
  }

  return res;
}

/**
 * @brief Expand left to recover the first matching suffix-array position.
 *
 * If neighboring suffixes share a common prefix of length at least len,
 * then they also match the query.
 *
 * @param pos One matching suffix-array position
 * @param len Query length
 * @return First matching suffix-array position
 */
size_t SuffixArray::_findFirstOccurrence(size_t pos, size_t len) const {
  while ((pos > 0) && (_lcp[pos - 1] >= len)) {
    --pos;
  }
  return pos;
}

/**
 * @brief Expand right to recover the last matching suffix-array position.
 *
 * If neighboring suffixes share a common prefix of length at least len,
 * then they also match the query.
 *
 * @param pos One matching suffix-array position
 * @param len Query length
 * @return Last matching suffix-array position
 */
size_t SuffixArray::_findLastOccurrence(size_t pos, size_t len) const {
  while ((pos < _lcp.size() - 1) && (_lcp[pos] >= len)) {
    ++pos;
  }
  return pos;
}

/**
 * @brief Construct suffix array and LCP array from a reference string.
 *
 * Construction steps:
 * 1. store the reference string
 * 2. create all suffix start positions from 0 to n
 * 3. sort them lexicographically
 * 4. compute the LCP values between consecutive suffixes
 *
 *
 * @param ref Reference string to index
 */
SuffixArray::SuffixArray(const string &ref)
  : _ref(ref), _sa(), _lcp() {
  size_t n = _ref.length();

  _sa.reserve(n + 1);
  for (size_t i = 0; i <= n; ++i) {
    _sa.push_back(i);
  }

  SuffixComparator comp(_ref);
  sort(_sa.begin(), _sa.end(), comp);

  _lcp.reserve(n + 1);
  for (size_t i = 0; i < n; ++i) {
    int cmp = _compareStrings(_ref, _sa[i], _ref, _sa[i + 1]);
    if (cmp < 0) {
      cmp = -cmp;
    }
    _lcp.push_back(static_cast<size_t>(cmp - 1));
  }
  _lcp.push_back(0);
}

/**
 * @brief Print the full suffix array structure.
 *
 * For each suffix-array entry, this method prints:
 * - the suffix-array index
 * - the start position in the reference
 * - the LCP value
 * - the suffix string itself
 *
 * @param os Output stream
 */
void SuffixArray::print(ostream &os) const {
  size_t n = _sa.size();

  os << "Indexed string: '" << _ref << "':" << endl;
  for (size_t i = 0; i < n; ++i) {
    os << "  - "
       << i
       << " [SA: " << _sa[i]
       << " / LCP: " << _lcp[i]
       << "]: '";
    printSuffix(_sa[i], os);
    os << "'" << endl;
  }
  os << endl;
}

/**
 * @brief Print the suffix starting at a given position.
 *
 * @param pos Start position in the reference
 * @param os Output stream
 */
void SuffixArray::printSuffix(size_t pos, ostream &os) const {
  while (pos < _ref.length()) {
    os << _ref[pos++];
  }
}

/**
 * @brief Check whether a query appears in the indexed reference.
 *
 * @param query Pattern to search
 * @return true if at least one occurrence exists
 * @return false otherwise
 */
bool SuffixArray::contains(const string &query) const {
  return (_findOccurrence(query) != static_cast<size_t>(-1));
}

/**
 * @brief Count exact occurrences of a query.
 *
 * Strategy:
 * 1. find one occurrence
 * 2. expand left and right using the LCP array
 * 3. compute interval size
 *
 * @param query Pattern to search
 * @return Number of exact occurrences
 */
size_t SuffixArray::nbOccurrences(const string &query) const {
  size_t p = _findOccurrence(query);
  size_t res = 0;

  if (p != static_cast<size_t>(-1)) {
    size_t m = query.length();
    size_t first = _findFirstOccurrence(p, m);
    size_t last = _findLastOccurrence(p, m);
    res = last - first + 1;
  }

  return res;
}

/**
 * @brief Return all exact occurrence positions of a query.
 *
 * Strategy:
 * 1. find one occurrence
 * 2. recover the full interval with the LCP array
 * 3. copy the corresponding suffix start positions
 *
 * @param query Pattern to search
 * @return Vector of exact occurrence positions
 */
vector<size_t> SuffixArray::occurrences(const string &query) const {
  size_t p = _findOccurrence(query);
  vector<size_t> res;

  if (p != static_cast<size_t>(-1)) {
    size_t m = query.length();
    size_t first = _findFirstOccurrence(p, m);
    size_t last = _findLastOccurrence(p, m);

    res.resize(last - first + 1);
    copy(_sa.begin() + first, _sa.begin() + last + 1, res.begin());
  }

  return res;
}

/**
 * @brief Return the indexed reference string.
 *
 * @return Reference string
 */
const string &SuffixArray::reference() const {
  return _ref;
}

/**
 * @brief Return the suffix array.
 *
 * @return Vector of sorted suffix start positions
 */
const vector<size_t> &SuffixArray::suffixes() const {
  return _sa;
}