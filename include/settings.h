/**
 * @file settings.h
 * @brief Definition of the Settings class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This class stores the main parameters of the mapping program.
 *
 * Instead of keeping command-line parameters spread across the main function,
 * they are grouped in one simple object.
 *
 * This makes the program easier to read and easier to configure.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

/**
 * @brief Container for program settings.
 *
 * This class stores:
 * - reference file path
 * - input directory path
 * - output file path
 * - maximum allowed mismatches
 * - k-mer size
 */
class Settings {
public:
  /**
   * @brief Reference FASTA file.
   */
  std::string reference_file;

  /**
   * @brief Directory containing input sequence files.
   */
  std::string input_directory;

  /**
   * @brief Output result file.
   */
  std::string output_file;

  /**
   * @brief Maximum number of mismatches allowed during mapping.
   */
  int max_mismatches;

  /**
   * @brief k-mer size used by the mapper.
   */
  int kmer_size;

  /**
   * @brief Construct default settings.
   *
   * Default values are chosen to keep the program simple to run.
   */
  Settings();

  /**
   * @brief Parse command-line arguments.
   *
   * Expected format:
   *   program <reference.fasta> <input_directory> or <file> [max_mismatches] [kmer_size]
   *
   * @param argc Number of arguments
   * @param argv Argument values
   * @return true if parsing succeeded
   * @return false otherwise
   */
  bool parse(int argc, char **argv);

  /**
   * @brief Print help message.
   *
   * @param program_name Name of the executable
   */
  void printHelp(const std::string &program_name) const;
};

#endif