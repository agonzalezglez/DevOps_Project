/**
 * @file fastq_file_reader.h
 * @brief FASTQ file reader definition.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This class specializes the FileReader interface for FASTQ files.
 *
 * FASTQ format description:
 * A FASTQ file stores biological sequences along with quality scores.
 *
 * Each sequence is represented by:
 *
 * 1. A header line starting with '@'
 * 2. One or more lines containing the sequence
 * 3. A separator line starting with '+'
 * 4. One or more lines containing quality scores
 *
 * Important:
 * - The sequence and quality sections may span multiple lines.
 * - The total length of the quality string must match the sequence length.
 *
 */

#ifndef FASTQ_FILE_READER_H
#define FASTQ_FILE_READER_H

#include "file_reader.h"
#include <string>

/**
 * @brief Specialized reader for FASTQ files.
 *
 * This class reads FASTQ records and converts them into
 * BiologicalSequence objects.
 *
 * It extends the generic FileReader by implementing:
 * - FASTQ format validation
 * - FASTQ-specific parsing 
 */
class FastqFileReader : public FileReader {
public:
  /**
   * @brief Construct a FASTQ reader.
   *
   * @param warn Enable or disable warnings
   */
  FastqFileReader(bool warn = true);

  /**
   * @brief Check whether the file appears to be in FASTQ format.
   *
   * This method performs a simple validation:
   * it checks whether the first meaningful character in the file is '@'.
   *
   * This is not a strict validation, but sufficient for basic detection.
   *
   * @return true if the file looks like FASTQ
   * @return false otherwise
   */
  bool isValidFormat() override;

  /**
   * @brief Read the next sequence from the FASTQ file.
   *
   * Parsing strategy:
   * - locate a header line starting with '@'
   * - accumulate sequence lines until '+' is found
   * - accumulate quality lines until lengths match the sequence
   *
   * The method returns a BiologicalSequence containing:
   * - description
   * - sequence
   * - quality scores
   *
   * Limitations:
   * - does not validate quality score encoding (ASCII range)
   * - assumes file is not corrupted
   *
   * @return Parsed BiologicalSequence, or an empty object if reading fails
   */
  BiologicalSequence nextSequence() override;
};

#endif