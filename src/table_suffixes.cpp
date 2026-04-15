/**
 * @file table_suffixes.cpp
 * @brief Small test program for the SuffixArray class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This file provides a command-line program used to visualize
 * and test the suffix array implementation.
 *
 * Steps:
 * - build a suffix array from an input string
 * - display the internal suffix array structure
 * - test exact pattern search on the indexed string
 *
 */

#include <iostream>

#include "suffix_array.h"

using namespace std;

/**
 * @brief Main function for suffix array testing.
 *
 * Expected usage:
 *   program <word> [query1 query2 ...]
 *
 * Behavior:
 * 1. read the input word from the command line
 * 2. build a suffix array from that word
 * 3. print the full suffix array structure
 * 4. for each additional query:
 *    - check whether it is present
 *    - count occurrences
 *    - print occurrence positions
 *    - print the suffixes starting at those positions
 *
 * This makes it easier to verify that:
 * - suffix sorting works correctly
 * - query search works correctly
 * - occurrence positions are coherent
 *
 * @param argc Number of command-line arguments
 * @param argv Argument values
 * @return Exit code
 */
int main(int argc, char **argv) {
  /**
   * At least one input string is required.
   */
  if (argc <= 1) {
    cerr << "usage: " << argv[0]
         << " <word> [query1 query2 ...]"
         << endl;
    return 1;
  }

  /**
   * Step 1: build suffix array from the input word.
   */
  string text = argv[1];
  SuffixArray sa(text);

  /**
   * Step 2: print the suffix array structure.
   *
   * This includes:
   * - suffix array positions
   * - LCP values
   * - visible suffix strings
   */
  cout << sa;

  /**
   * Step 3: evaluate each query given after the input word.
   */
  for (int i = 2; i < argc; ++i) {
    string pattern = argv[i];

    cout << "Query string: '" << pattern << "'" << endl;
    cout << " - contains: " << sa.contains(pattern) << endl;
    cout << " - nb occurrences: " << sa.nbOccurrences(pattern) << endl;

    vector<size_t> positions = sa.occurrences(pattern);

    cout << " - positions:";
    if (positions.empty()) {
      cout << " <empty>";
    }
    cout << endl;

    /**
     * For each occurrence position, print:
     * - the position in the indexed string
     * - the suffix starting at that position
     *
     * This is useful to visually verify the correctness of the match.
     */
    for (size_t j = 0; j < positions.size(); ++j) {
      cout << "   - " << positions[j] << " -> '";
      sa.printSuffix(positions[j]);
      cout << "'" << endl;
    }

    cout << endl;
  }

  return 0;
}