/**
 * @file settings.cpp
 * @brief Implementation of the Settings class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This file implements a command-line parameter handler.
 *
 */

#include "settings.h"

#include <cstdlib>
#include <iostream>

using namespace std;

/**
 * @brief Construct default settings.
 *
 * Default values:
 * - output file: output/results/mapping_results.tsv
 * - maximum mismatches: 4
 * - k-mer size: 9
 *
 * These values can later be overridden by command-line arguments.
 */
Settings::Settings():
  reference_file(),
  input_directory(),
  output_file("output/results/mapping_results.tsv"),
  max_mismatches(4),
  kmer_size(9) {
}

/**
 * @brief Parse command-line arguments.
 *
 * Expected usage:
 *   program <reference.fasta> <input_directory/file> [max_mismatches] [kmer_size]
 *
 * If optional arguments are missing or invalid, default values are kept.
 *
 * @param argc Number of arguments
 * @param argv Argument values
 * @return true if parsing succeeded
 * @return false otherwise
 */
bool Settings::parse(int argc, char **argv) {
  if (argc < 3 || argc > 5) {
    return false;
  }

  reference_file = argv[1];
  input_directory = argv[2];

  if (argc >= 4) {
    max_mismatches = atoi(argv[3]);
    if (max_mismatches < 0) {
      max_mismatches = 4;
    }
  }

  if (argc >= 5) {
    kmer_size = atoi(argv[4]);
    if (kmer_size <= 0) {
      kmer_size = 9;
    }
  }

  return true;
}

/**
 * @brief Print command-line help message.
 *
 * @param program_name Executable name
 */
void Settings::printHelp(const string &program_name) const {
  cerr << "Usage: " << program_name
       << " <reference.fasta> <input_directory> [max_mismatches] [kmer_size]"
       << endl;
}