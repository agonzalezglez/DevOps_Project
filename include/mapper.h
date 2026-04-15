/**
 * @file mapper.h
 * @brief K-mer based read mapper.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * Steps:
 * - split the read into k-mers
 * - search each k-mer in the indexed reference
 * - infer candidate read positions
 * - verify candidates using the full read
 */

#ifndef MAPPER_H
#define MAPPER_H

#include "biological_sequence.h"
#include "reference_index.h"

#include <string>
#include <vector>

/**
 * @brief Class read mapper.
 */
class Mapper {
public:
  /**
   * @brief Result of one candidate mapping.
   */
  struct MappingResult {
    int position;          /**< Candidate position on the reference */
    bool isReverse;        /**< true if mapping is on reverse strand */
    double score;          /**< Confidence score */
    int mismatches;        /**< Number of mismatches */
    std::string cigar;     /**< Simplified CIGAR string */
    int matchedKmers;      /**< Number of supporting k-mers */
    int totalKmers;        /**< Total number of extracted k-mers */
  };

  /**
   * @brief Map one read to the indexed reference.
   *
   * @param read Input read sequence
   * @param index Indexed reference
   * @param maxMismatches Maximum allowed mismatches
   * @return Sorted list of candidate mapping results
   */
  static std::vector<MappingResult> mapRead(const BiologicalSequence &read,
                                            const ReferenceIndex &index,
                                            int maxMismatches = 2);

private:
  /**
   * @brief Extract all k-mers from a sequence.
   *
   * k-mers containing N are ignored.
   *
   * @param sequence Input sequence
   * @param k k-mer size
   * @return List of extracted k-mers
   */
  static std::vector<std::string> extractKmers(const std::string &sequence, int k);

  /**
   * @brief Check whether a candidate position is valid.
   *
   * @param read Read sequence
   * @param reference Reference sequence
   * @param position Candidate position
   * @return true if read fits inside the reference
   * @return false otherwise
   */
  static bool verifyCandidate(const std::string &read,
                              const std::string &reference,
                              int position);

  /**
   * @brief Count mismatches between a read and the reference.
   *
   * @param read Read sequence
   * @param reference Reference sequence
   * @param position Candidate start position
   * @return Number of mismatches, or max int if invalid
   */
  static int countMismatches(const std::string &read,
                             const std::string &reference,
                             int position);

  /**
   * @brief Generate a simplified CIGAR string.
   *
   * @param read Read sequence
   * @param reference Reference sequence
   * @param position Candidate start position
   * @return Simplified CIGAR string
   */
  static std::string generateCigar(const std::string &read,
                                   const std::string &reference,
                                   int position);

  /**
   * @brief Compute a mapping score.
   *
   * @param matchingKmers Number of supporting k-mers
   * @param totalKmers Total number of k-mers
   * @param mismatches Number of mismatches
   * @return Mapping score
   */
  static double calculateMappingScore(int matchingKmers,
                                      int totalKmers,
                                      int mismatches);

  /**
   * @brief Compute reverse complement of a sequence.
   *
   * @param seq Input sequence
   * @return Reverse complement
   */
  static std::string getReverseComplement(const std::string &seq);

  /**
   * @brief Normalize one nucleotide.
   *
   * @param c Input character
   * @return Normalized nucleotide
   */
  static char normalizeBase(char c);

  /**
   * @brief Normalize a full sequence.
   *
   * @param seq Input sequence
   * @return Normalized sequence
   */
  static std::string normalizeSequence(const std::string &seq);
};

#endif