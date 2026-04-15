/**
 * @file fasta_file_reader.cpp
 * @brief Implementation of the FastaFileReader class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This file implements a reader for FASTA sequence files.
 *
 * Steps:
 * - locate a header line
 * - accumulate all following sequence lines
 * - stop when a new header is found
 *
 * The parsed sequence is returned as a BiologicalSequence object.
 *
 */

#include "fasta_file_reader.h"

#include <cctype>
#include <iostream>

using namespace std;

/**
 * @brief Remove leading and trailing whitespace from a FASTA line.
 *
 * This helper function is used before interpreting a line as a header
 * or as part of the sequence.
 *
 * @param s Input line
 * @return Trimmed line
 */
static string trim_fasta(const string &s) {
  size_t start = 0;
  while ((start < s.size()) && isspace(static_cast<unsigned char>(s[start]))) {
    ++start;
  }

  size_t end = s.size();
  while ((end > start) && isspace(static_cast<unsigned char>(s[end - 1]))) {
    --end;
  }

  return s.substr(start, end - start);
}

/**
 * @brief Check whether a line is a FASTA header.
 *
 * In the most common FASTA format, headers begin with '>'.
 * This implementation also accepts ';' because it can appear in older or
 * course-related examples.
 *
 * @param line Input line
 * @return true if the line is interpreted as a FASTA header
 * @return false otherwise
 */
static bool is_fasta_header_line(const string &line) {
  if (line.empty()) {
    return false;
  }

  return (line[0] == '>') || (line[0] == ';');
}

/**
 * @brief Construct a FASTA reader.
 *
 * The constructor initializes both:
 * - the generic FileReader state
 * - the FASTA-specific pending header mechanism
 *
 * @param warn Enable or disable warnings
 */
FastaFileReader::FastaFileReader(bool warn)
  : FileReader(warn),
    _has_pending_header(false),
    _pending_header_line() {
}

/**
 * @brief Open a FASTA file and reset internal FASTA state.
 *
 * This method clears any previously stored pending header and then
 * delegates file opening to the base FileReader class.
 *
 * @param filename File to open
 * @param sequence_type Expected biological sequence type
 * @return true if opening succeeded
 * @return false otherwise
 */
bool FastaFileReader::open(const string &filename,
                           BiologicalSequence::Type sequence_type) {
  _has_pending_header = false;
  _pending_header_line.clear();

  return FileReader::open(filename, sequence_type);
}

/**
 * @brief Check whether the current file looks like a FASTA file.
 *
 * Steps:
 * - save current file position
 * - scan until the first non-empty meaningful line
 * - check whether it starts with '>' or ';'
 * - restore the previous file position
 *
 *
 * @return true if the file appears to be FASTA
 * @return false otherwise
 */
bool FastaFileReader::isValidFormat() {
  if (!_ifs.is_open()) {
    return false;
  }

  streampos pos = _ifs.tellg();
  size_t saved_line = _line;
  size_t saved_column = _column;

  string line;
  bool found = false;

  while (getline(_ifs, line)) {
    string trimmed = trim_fasta(line);

    if (trimmed.empty()) {
      continue;
    }

    found = is_fasta_header_line(trimmed);
    break;
  }

  _ifs.clear();
  _ifs.seekg(pos);
  _line = saved_line;
  _column = saved_column;

  return found;
}

/**
 * @brief Read the next sequence from the FASTA file.
 *
 * Steps:
 * 1. find the header of the next sequence
 * 2. remove the leading header marker ('>' or ';')
 * 3. accumulate sequence lines until:
 *    - another header is found
 *    - or the end of file is reached
 * 4. build a BiologicalSequence object from the collected data
 *
 * The method also uses a "pending header" mechanism:
 * if the header of the next sequence is encountered while reading the current
 * sequence, it is stored and reused during the next call.
 *
 * Sequence cleaning behavior:
 * - empty lines are ignored
 * - whitespace inside sequence lines is ignored
 * - letters are converted to uppercase
 * - characters must belong to the extended DNA alphabet
 * - if ';' appears inside a sequence line, the rest of that line is ignored
 *
 * Default type behavior:
 * - if no type was explicitly requested, EXTENDED_DNA is used by default
 *
 * @return Parsed BiologicalSequence, or an empty object if no sequence remains
 */
BiologicalSequence FastaFileReader::nextSequence() {
  BiologicalSequence result;

  if (!_ifs.is_open()) {
    return result;
  }

  string header;
  string line;
  bool found_header = false;

  /**
   * Step 1: determine which header to use.
   *
   * Either:
   * - reuse a pending header already read previously
   * - or search for the next header in the file
   */
  if (_has_pending_header) {
    header = _pending_header_line;
    _has_pending_header = false;
    _pending_header_line.clear();
    found_header = true;
  } else {
    while (getline(_ifs, line)) {
      ++_line;
      _column = 0;

      string trimmed = trim_fasta(line);

      if (trimmed.empty()) {
        continue;
      }

      if (is_fasta_header_line(trimmed)) {
        header = trimmed;
        found_header = true;
        break;
      }
    }
  }

  /**
   * If no header is found, there is no sequence left to read.
   */
  if (!found_header) {
    return result;
  }

  /**
   * Remove the header marker and clean the description.
   */
  if (!header.empty()) {
    header = trim_fasta(header.substr(1));
  }

  string sequence;

  /**
   * Step 2: read sequence lines until the next header or end of file.
   */
  while (getline(_ifs, line)) {
    ++_line;
    _column = 0;

    string trimmed = trim_fasta(line);

    if (trimmed.empty()) {
      continue;
    }

    /**
     * If a new header is found, it belongs to the next sequence.
     * Store it for the next call and stop reading the current sequence.
     */
    if (is_fasta_header_line(trimmed)) {
      _has_pending_header = true;
      _pending_header_line = trimmed;
      break;
    }

    /**
     * Process current sequence line character by character.
     *
     * This allows:
     * - ignoring internal whitespace
     * - converting to uppercase
     * - stopping if ';' is used as an inline comment marker
     * - rejecting invalid symbols
     */
    for (size_t i = 0; i < trimmed.size(); ++i) {
      char c = trimmed[i];
      _column = i + 1;

      if (isspace(static_cast<unsigned char>(c))) {
        continue;
      }

      /**
       * In some FASTA variants, ';' may start a comment inside a line.
       * In this implementation, the rest of the line is ignored.
       */
      if (c == ';') {
        break;
      }

      if (BiologicalSequence::isExtendedDNA(c)) {
        sequence.push_back(static_cast<char>(toupper(static_cast<unsigned char>(c))));
      } else if (_warn) {
        cerr << "[WARNING] FASTA: symbol '" << c
             << "' ignored in file '" << _filename
             << "' at line " << _line
             << ", column " << _column << endl;
      }
    }
  }

  /**
   * Step 3: build the final BiologicalSequence object.
   */
  result.setDescription(header);
  result.setSequence(sequence);

  /**
   * If no type was explicitly requested, use EXTENDED_DNA by default.
   * This is a convenient default for genomic FASTA files.
   */
  BiologicalSequence::Type final_type =
    (_sequence_type == BiologicalSequence::UNKNOWN)
      ? BiologicalSequence::EXTENDED_DNA
      : _sequence_type;

  /**
   * If the sequence is incompatible with the requested type,
   * emit a warning if warnings are enabled.
   */
  if (!result.setType(final_type) && _warn) {
    cerr << "[WARNING] FASTA: incompatible biological type for sequence '"
         << result.getDescription() << "'" << endl;
  }

  return result;
}
