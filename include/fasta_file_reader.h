/**
 * @file fasta_file_reader.h
 * @brief FASTA file reader implementation.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * A FASTA file contains one or more biological sequences.
 *
 * General structure:
 * - a header line beginning with '>'
 * - one or more sequence lines
 *
 * In some historical cases, ';' may also appear as a header marker.
 * This reader accepts both '>' and ';' as header line starters.
 *
 * This class specializes the generic FileReader interface for FASTA files.
 */

#ifndef FASTA_FILE_READER_H
#define FASTA_FILE_READER_H

#include "file_reader.h"
#include <string>

/**
 * @brief Specialized reader for FASTA files.
 *
 * This class reads one FASTA sequence at a time and returns it as a
 * BiologicalSequence object.
 *
 * It also keeps a pending header when the next sequence header is read
 * before the current parsing step is finished.
 */
class FastaFileReader : public FileReader {
private:
  /**
   * @brief Indicates whether the next header was already read.
   *
   * During FASTA parsing, the reader may encounter the header of the next
   * sequence while still finishing the current one. In that case, the header
   * is stored and reused on the next call to nextSequence().
   */
  bool _has_pending_header;

  /**
   * @brief Stored header line for the next sequence.
   */
  std::string _pending_header_line;

public:
  /**
   * @brief Construct a FASTA reader.
   *
   * @param warn Enable or disable warnings
   */
  FastaFileReader(bool warn = true);

  /**
   * @brief Open a FASTA file and reset FASTA-specific internal state.
   *
   * In addition to the generic FileReader initialization, this method
   * resets the pending-header mechanism.
   *
   * @param filename File to open
   * @param sequence_type Expected biological sequence type
   * @return true if opening succeeded
   * @return false otherwise
   */
  bool open(const std::string &filename,
            BiologicalSequence::Type sequence_type = BiologicalSequence::UNKNOWN) override;

  /**
   * @brief Check whether the current file appears to be in FASTA format.
   *
   * This method performs a simple format check by searching for the first
   * non-empty line and verifying that it begins with '>' or ';'.
   *
   * @return true if the file looks like FASTA
   * @return false otherwise
   */
  bool isValidFormat() override;

  /**
   * @brief Read the next FASTA sequence from the file.
   *
   * Steps:
   * - find the next header line
   * - collect sequence lines until the next header or end of file
   * - build and return a BiologicalSequence object
   *
   * @return Parsed sequence, or an empty BiologicalSequence if none is available
   */
  BiologicalSequence nextSequence() override;
};

#endif