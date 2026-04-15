/**
 * @file file_reader.h
 * @brief Abstract base class for biological sequence file readers.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This class defines a common interface for reading biological sequence files,
 * such as FASTA and FASTQ.
 *
 * FASTA and FASTQ formats have different structures, but they share the same goal:
 * reading biological sequences from files.
 *
 * This class provides:
 * - file management (open/close)
 * - reading position tracking (line, column)
 * - warning control
 * - biological sequence type handling
 *
 * Derived classes must implement:
 * - format validation
 * - sequence parsing
 */

#ifndef FILE_READER_H
#define FILE_READER_H

#include <fstream>
#include <string>

#include "biological_sequence.h"

/**
 * @brief Abstract base class for sequence file readers.
 *
 * This class provides shared functionality for all file readers and defines
 * a common interface for reading biological sequences.
 *
 * It is designed to be extended by specialized readers such as:
 * - FastaFileReader
 * - FastqFileReader
 */
class FileReader {
protected:
  /**
   * @brief Input file stream.
   */
  std::ifstream _ifs;

  /**
   * @brief Name of the current file.
   */
  std::string _filename;

  /**
   * @brief Current line number in the file.
   *
   * Used for debugging and error reporting.
   */
  size_t _line;

  /**
   * @brief Current column number in the file.
   *
   * Useful for precise error localization.
   */
  size_t _column;

  /**
   * @brief Enable or disable warnings.
   */
  bool _warn;

  /**
   * @brief Expected biological sequence type.
   */
  BiologicalSequence::Type _sequence_type;

public:
  /**
   * @brief Construct a FileReader.
   *
   * @param warn Enable or disable warnings
   */
  FileReader(bool warn = true);

  /**
   * @brief Virtual destructor.
   *
   * Required for correct polymorphic behavior when deleting derived objects
   * through a base class pointer.
   */
  virtual ~FileReader();

  /**
   * @brief Open a file and initialize internal state.
   *
   * This method:
   * - closes any previously opened file
   * - opens the new file
   * - resets reading position
   *
   * @param filename File to open
   * @param sequence_type Expected biological type
   * @return true if the file was successfully opened
   * @return false otherwise
   */
  virtual bool open(const std::string &filename,
                    BiologicalSequence::Type sequence_type = BiologicalSequence::UNKNOWN);

  /**
   * @brief Close the current file and reset internal state.
   */
  virtual void close();

  /**
   * @brief Get the current filename.
   *
   * @return Filename string
   */
  std::string filename() const;

  /**
   * @brief Get current line number.
   *
   * @return Line index
   */
  size_t line() const;

  /**
   * @brief Get current column number.
   *
   * @return Column index
   */
  size_t column() const;

  /**
   * @brief Check whether warnings are enabled.
   *
   * @return true if warnings are enabled
   */
  bool warn() const;

  /**
   * @brief Enable or disable warnings.
   *
   * @param value New warning state
   */
  void warn(bool value);

  /**
   * @brief Get expected biological sequence type.
   *
   * @return Sequence type
   */
  BiologicalSequence::Type sequenceType() const;

  /**
   * @brief Set expected biological sequence type.
   *
   * @param type Sequence type
   */
  void sequenceType(BiologicalSequence::Type type);

  /**
   * @brief Check whether a file is currently open.
   *
   * @return true if a file is open
   */
  bool isOpen() const;

  /**
   * @brief Check if the file format is valid for this reader.
   *
   * Must be implemented by derived classes.
   *
   * @return true if the format is valid
   * @return false otherwise
   */
  virtual bool isValidFormat() = 0;

  /**
   * @brief Read the next biological sequence from the file.
   *
   * Must be implemented by derived classes.
   *
   * @return BiologicalSequence object
   */
  virtual BiologicalSequence nextSequence() = 0;

  /**
   * @brief Find the first meaningful character in a file.
   *
   * A meaningful character is the first non-whitespace character.
   *
   * This is useful for quick format detection.
   *
   * @param filename File to inspect
   * @return First meaningful character, or '\0' if none found
   */
  static char firstMeaningfulChar(const std::string &filename);
};

#endif