/**
 * @file file_reader.cpp
 * @brief Implementation of the FileReader base class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This file provides the common functionality shared by all file readers.
 *
 * - manages file opening and closing
 * - tracks reading position
 * - handles warning settings
 * - provides utility methods
 */

#include "file_reader.h"
#include <cctype>
#include <fstream>

using namespace std;

/**
 * @brief Construct a FileReader.
 *
 * Initializes:
 * - no file opened
 * - line and column set to 0
 * - warning mode according to parameter
 */
FileReader::FileReader(bool warn) {
  _filename = "";
  _line = 0;
  _column = 0;
  _warn = warn;
  _sequence_type = BiologicalSequence::UNKNOWN;
}

/**
 * @brief Destructor.
 *
 * Ensures the file is properly closed when the object is destroyed.
 */
FileReader::~FileReader() {
  close();
}

/**
 * @brief Open a file and reset internal state.
 *
 * This method:
 * - closes any previously opened file
 * - opens the new file
 * - resets position counters
 *
 * @param filename File to open
 * @param sequence_type Expected biological type
 * @return true if successful
 * @return false otherwise
 */
bool FileReader::open(const string &filename,
                      BiologicalSequence::Type sequence_type) {
  close();

  _ifs.open(filename.c_str());
  if (!_ifs.is_open()) {
    return false;
  }

  _filename = filename;
  _line = 0;
  _column = 0;
  _sequence_type = sequence_type;

  return true;
}

/**
 * @brief Close the file and reset internal state.
 */
void FileReader::close() {
  if (_ifs.is_open()) {
    _ifs.close();
  }

  _filename = "";
  _line = 0;
  _column = 0;
  _sequence_type = BiologicalSequence::UNKNOWN;
}

/**
 * @brief Return the current filename.
 */
string FileReader::filename() const {
  return _filename;
}

/**
 * @brief Return the current line number.
 */
size_t FileReader::line() const {
  return _line;
}

/**
 * @brief Return the current column number.
 */
size_t FileReader::column() const {
  return _column;
}

/**
 * @brief Check if warnings are enabled.
 */
bool FileReader::warn() const {
  return _warn;
}

/**
 * @brief Set warning mode.
 */
void FileReader::warn(bool value) {
  _warn = value;
}

/**
 * @brief Get expected biological sequence type.
 */
BiologicalSequence::Type FileReader::sequenceType() const {
  return _sequence_type;
}

/**
 * @brief Set expected biological sequence type.
 */
void FileReader::sequenceType(BiologicalSequence::Type type) {
  _sequence_type = type;
}

/**
 * @brief Check if a file is currently open.
 */
bool FileReader::isOpen() const {
  return _ifs.is_open();
}

/**
 * @brief Find the first meaningful character in a file.
 *
 * This method reads the file from the beginning and returns
 * the first non-whitespace character.
 *
 * It is used for quick format detection (FASTA vs FASTQ).
 *
 * @param filename File to inspect
 * @return First non-whitespace character or '\0' if not found
 */
char FileReader::firstMeaningfulChar(const string &filename) {
  ifstream ifs(filename.c_str());
  if (!ifs.is_open()) {
    return '\0';
  }

  char c;
  while (ifs.get(c)) {
    if (!isspace(static_cast<unsigned char>(c))) {
      return c;
    }
  }

  return '\0';
}