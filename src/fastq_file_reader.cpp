/**
 * @file fastq_file_reader.cpp
 * @brief Implementation of the FastqFileReader class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This file implements a FASTQ reader.
 *
 * FASTQ logical structure:
 * 1. @description
 * 2. sequence (one or more lines)
 * 3. +
 * 4. quality (one or more lines)
 *
 * The quality section ends when its length matches the sequence length.
 */

#include "fastq_file_reader.h"

#include <cctype>
#include <iostream>

using namespace std;

/**
 * @brief Remove leading and trailing whitespace from a FASTQ line.
 *
 * @param s Input line
 * @return Trimmed line
 */
static string trim_fastq(const string &s) {
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
 * @brief Construct a FASTQ reader.
 *
 * @param warn Enable or disable warnings
 */
FastqFileReader::FastqFileReader(bool warn)
  : FileReader(warn) {
}

/**
 * @brief Check whether the file appears to be in FASTQ format.
 *
 * The first meaningful character should be '@'.
 *
 * @return true if the format looks like FASTQ
 * @return false otherwise
 */
bool FastqFileReader::isValidFormat() {
  if (!_ifs.is_open()) {
    return false;
  }

  char c = firstMeaningfulChar(_filename);
  return (c == '@');
}

/**
 * @brief Read the next sequence from a FASTQ file.
 *
 * Steps:
 * 1. find the description line starting with '@'
 * 2. accumulate sequence lines until '+' is found
 * 3. accumulate quality lines until quality length matches sequence length
 *
 *
 * @return Parsed BiologicalSequence, or an empty object if reading fails
 */
BiologicalSequence FastqFileReader::nextSequence() {
  BiologicalSequence result;

  if (!_ifs.is_open()) {
    return result;
  }

  string line;

  /**
   * Step 1: find the description line
   */
  while (getline(_ifs, line)) {
    ++_line;
    _column = 0;

    line = trim_fastq(line);

    if (line.empty()) {
      continue;
    }

    if (line[0] == '@') {
      break;
    }
  }

  if (line.empty() || line[0] != '@') {
    return result;
  }

  string description = line.substr(1);

  /**
   * Step 2: read sequence lines until '+' line
   */
  string sequence;
  string plus_line;
  bool found_plus = false;

  while (getline(_ifs, line)) {
    ++_line;
    _column = 0;

    line = trim_fastq(line);

    if (line.empty()) {
      continue;
    }

    if (line[0] == '+') {
      plus_line = line;
      found_plus = true;
      break;
    }

    sequence += line;
  }

  if (!found_plus) {
    if (_warn) {
      cerr << "[WARNING] FASTQ: missing '+' line before end of file" << endl;
    }
    return result;
  }

  /**
   * Step 3: read quality lines until lengths match
   */
  string quality;

  while (quality.size() < sequence.size() && getline(_ifs, line)) {
    ++_line;
    _column = 0;

    line = trim_fastq(line);

    if (line.empty()) {
      continue;
    }

    quality += line;
  }

  /**
   * Validate sequence / quality length consistency
   */
  if (quality.size() != sequence.size()) {
    if (_warn) {
      cerr << "[WARNING] FASTQ: quality length differs from sequence length for sequence '"
           << description << "'" << endl;
    }
    return result;
  }

  /**
   * Build biological sequence object
   */
  result.setDescription(description);
  result.setSequence(sequence);
  result.setQuality(quality);

  /**
   * Set biological type
   */
  BiologicalSequence::Type final_type =
    (_sequence_type == BiologicalSequence::UNKNOWN)
      ? BiologicalSequence::EXTENDED_DNA
      : _sequence_type;

  if (!result.setType(final_type) && _warn) {
    cerr << "[WARNING] FASTQ: incompatible type for sequence '"
         << result.getDescription() << "'" << endl;
    return BiologicalSequence();
  }

  return result;
}