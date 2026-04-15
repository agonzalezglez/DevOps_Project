/**
 * @file mapper.cpp
 * @brief Implementation of the Mapper class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This file contains the main mapping logic of the project.
 *
 * - the reference genome is already indexed
 * - each read is split into k-mers
 * - each k-mer is searched in the reference
 * - every hit suggests a possible start position for the full read
 * - candidate positions are then checked using the complete read
 */

#include "mapper.h"

#include <algorithm>
#include <limits>
#include <unordered_map>

using namespace std;

/**
 * @brief Check whether a k-mer contains the symbol 'N'.
 *
 * K-mers containing 'N' are ignored because they are
 * considered less informative for exact mapping.
 *
 * @param kmer Input k-mer
 * @return true if the k-mer contains at least one 'N'
 * @return false otherwise
 */
static bool containsN(const string &kmer) {
  for (size_t i = 0; i < kmer.size(); ++i) {
    if (kmer[i] == 'N') {
      return true;
    }
  }

  return false;
}

/**
 * @brief Normalize one nucleotide.
 *
 * This function simplifies the alphabet used by the mapper:
 * - lowercase letters are converted to uppercase
 * - U is converted to T
 * - unknown symbols are converted to N
 *
 * This makes sequence comparisons more consistent.
 *
 * @param c Input character
 * @return Normalized nucleotide
 */
char Mapper::normalizeBase(char c) {
  switch (c) {
  case 'a':
  case 'A':
    return 'A';
  case 'c':
  case 'C':
    return 'C';
  case 'g':
  case 'G':
    return 'G';
  case 't':
  case 'T':
    return 'T';
  case 'u':
  case 'U':
    return 'T';
  case 'n':
  case 'N':
    return 'N';
  default:
    return 'N';
  }
}

/**
 * @brief Normalize a full sequence.
 *
 * This function applies normalizeBase() to every character of the sequence.
 *
 * @param seq Input sequence
 * @return Normalized sequence
 */
string Mapper::normalizeSequence(const string &seq) {
  string result;
  result.reserve(seq.size());

  for (size_t i = 0; i < seq.size(); ++i) {
    result.push_back(normalizeBase(seq[i]));
  }

  return result;
}

/**
 * @brief Extract all overlapping k-mers from a sequence.
 *
 * Example:
 * if the sequence is ACGTAC and k = 3, the extracted k-mers are:
 * - ACG
 * - CGT
 * - GTA
 * - TAC
 *
 * K-mers containing 'N' are ignored because they are not useful for exact
 * pattern search in the current implementation.
 *
 * This function is important because it transforms one full read into a set
 * of short exact patterns that can be searched in the indexed reference.
 *
 * @param sequence Input sequence
 * @param k k-mer size
 * @return Vector of extracted k-mers
 */
vector<string> Mapper::extractKmers(const string &sequence, int k) {
  vector<string> kmers;
  string normalized = normalizeSequence(sequence);

  if (k <= 0) {
    return kmers;
  }

  if (normalized.size() < static_cast<size_t>(k)) {
    return kmers;
  }

  for (size_t i = 0; i <= normalized.size() - static_cast<size_t>(k); ++i) {
    string current = normalized.substr(i, k);

    if (!containsN(current)) {
      kmers.push_back(current);
    }
  }

  return kmers;
}

/**
 * @brief Check whether a candidate mapping position is valid.
 *
 * A candidate position is valid if:
 * - it is not negative
 * - the full read fits inside the reference from that position
 *
 * It only checks coordinate validity.
 *
 * @param read Read sequence
 * @param reference Reference sequence
 * @param position Candidate start position
 * @return true if the candidate position is valid
 * @return false otherwise
 */
bool Mapper::verifyCandidate(const string &read,
                             const string &reference,
                             int position) {
  if (position < 0) {
    return false;
  }

  if (position + static_cast<int>(read.size()) > static_cast<int>(reference.size())) {
    return false;
  }

  return true;
}

/**
 * @brief Count mismatches between a read and the reference at a given position.
 *
 * Once a candidate position has been inferred from k-mer hits, this function
 * compares the complete read to the reference at that position.
 *
 * This comparison is ungapped:
 * - insertions and deletions are not handled
 * - only position-by-position substitutions are counted
 *
 * If the candidate position is invalid, the function returns a very large
 * value so that the candidate can be rejected later.
 *
 * @param read Read sequence
 * @param reference Reference sequence
 * @param position Candidate start position
 * @return Number of mismatches, or a large value if invalid
 */
int Mapper::countMismatches(const string &read,
                            const string &reference,
                            int position) {
  string normalizedRead = normalizeSequence(read);
  string normalizedReference = normalizeSequence(reference);

  if (!verifyCandidate(normalizedRead, normalizedReference, position)) {
    return numeric_limits<int>::max();
  }

  int mismatches = 0;

  for (size_t i = 0; i < normalizedRead.size(); ++i) {
    if (normalizedRead[i] != normalizedReference[position + static_cast<int>(i)]) {
      ++mismatches;
    }
  }

  return mismatches;
}

/**
 * @brief Generate a simplified CIGAR string.
 *
 * In this mapper, the CIGAR string is intentionally minimal:
 * - if the candidate is valid, the full read is represented as nM
 * - if the candidate is invalid, the read is represented as nI
 *
 *
 * @param read Read sequence
 * @param reference Reference sequence
 * @param position Candidate start position
 * @return Simplified CIGAR string
 */
string Mapper::generateCigar(const string &read,
                             const string &reference,
                             int position) {
  string normalizedRead = normalizeSequence(read);
  string normalizedReference = normalizeSequence(reference);

  if (!verifyCandidate(normalizedRead, normalizedReference, position)) {
    return to_string(normalizedRead.size()) + "I";
  }

  return to_string(normalizedRead.size()) + "M";
}

/**
 * @brief Compute a mapping score.
 *
 * The current score combines two ideas:
 * - support from k-mers: more supporting k-mers means a better candidate
 * - mismatch penalty: more mismatches means a worse candidate
 *
 * The score is computed as:
 *   support_ratio - mismatch_penalty
 *
 * where:
 * - support_ratio = matchingKmers / totalKmers
 * - mismatch_penalty = mismatches / totalKmers
 *
 * Negative values are clipped to 0.
 *
 *
 * @param matchingKmers Number of k-mers supporting the candidate
 * @param totalKmers Total number of extracted k-mers
 * @param mismatches Number of mismatches in full-read verification
 * @return Mapping score
 */
double Mapper::calculateMappingScore(int matchingKmers,
                                     int totalKmers,
                                     int mismatches) {
  if (totalKmers <= 0) {
    return 0.0;
  }

  double support = static_cast<double>(matchingKmers) / static_cast<double>(totalKmers);
  double penalty = static_cast<double>(mismatches) / static_cast<double>(totalKmers);

  double score = support - penalty;

  if (score < 0.0) {
    score = 0.0;
  }

  return score;
}

/**
 * @brief Compute the reverse complement of a sequence.
 *
 * This function is used because a read may map either:
 * - on the forward strand
 * - or on the reverse-complement strand
 *
 * The sequence is first normalized, then reversed, and each nucleotide
 * is replaced by its complement.
 *
 * @param seq Input sequence
 * @return Reverse complement sequence
 */
string Mapper::getReverseComplement(const string &seq) {
  string normalized = normalizeSequence(seq);
  string result;
  result.reserve(normalized.size());

  for (string::const_reverse_iterator it = normalized.rbegin(); it != normalized.rend(); ++it) {
    switch (*it) {
    case 'A':
      result.push_back('T');
      break;
    case 'T':
      result.push_back('A');
      break;
    case 'C':
      result.push_back('G');
      break;
    case 'G':
      result.push_back('C');
      break;
    case 'N':
    default:
      result.push_back('N');
      break;
    }
  }

  return result;
}

/**
 * @brief Map one read to the indexed reference.
 *
 * This is the main algorithm of the mapper.
 *
 * Steps:
 * 1. get the reference and k-mer size from the index
 * 2. normalize the read
 * 3. create two versions of the read:
 *    - forward
 *    - reverse complement
 * 4. split each version into overlapping k-mers
 * 5. search each k-mer in the reference
 * 6. transform each k-mer hit into a candidate read start position
 * 7. count how many k-mers support each candidate position
 * 8. keep candidates with enough support
 * 9. verify these candidates with the full read
 * 10. compute score and store accepted mappings
 *
 * Candidate position logic:
 * if k-mer i of the read is found at position p in the reference,
 * then the read could start at:
 *   p - i
 *
 * because k-mer i begins at offset i inside the read.
 *
 * This method is based on k-mers that vote for the most coherent read start positions.
 *
 * Limitations:
 * - no gapped alignment
 * - no indel detection
 * - no use of FASTQ quality scores
 * - exact seed matching only
 *
 * @param read Input read
 * @param index Indexed reference
 * @param maxMismatches Maximum number of mismatches allowed
 * @return Vector of candidate mappings sorted by decreasing score
 */
vector<Mapper::MappingResult> Mapper::mapRead(const BiologicalSequence &read,
                                              const ReferenceIndex &index,
                                              int maxMismatches) {
  vector<MappingResult> results;

  const string reference = normalizeSequence(index.reference());
  const int k = index.k();

  string readForward = normalizeSequence(read.getSequence());
  string readReverse = getReverseComplement(readForward);

  vector<pair<string, bool>> readVersions;
  readVersions.push_back(make_pair(readForward, false));
  readVersions.push_back(make_pair(readReverse, true));

  for (size_t versionIndex = 0; versionIndex < readVersions.size(); ++versionIndex) {
    const string &currentRead = readVersions[versionIndex].first;
    bool isReverse = readVersions[versionIndex].second;

    vector<string> kmers = extractKmers(currentRead, k);
    if (kmers.empty()) {
      continue;
    }

    /**
     * @brief Count how many k-mers support each candidate position.
     *
     * Key:
     *   candidate start position on the reference
     * Value:
     *   number of supporting k-mers
     */
    unordered_map<int, int> positionCounts;

    for (size_t i = 0; i < kmers.size(); ++i) {
      vector<int> positions = index.getPositions(kmers[i]);

      for (size_t j = 0; j < positions.size(); ++j) {
        int candidateStart = positions[j] - static_cast<int>(i);

        if (candidateStart < 0) {
          continue;
        }

        positionCounts[candidateStart]++;
      }
    }

    /**
     * @brief Minimum support threshold for candidate acceptance.
     *
     * A candidate must be supported by at least half of the extracted k-mers.
     */
    int minSupport = max(1, static_cast<int>(kmers.size() / 2));

    for (unordered_map<int, int>::const_iterator it = positionCounts.begin();
         it != positionCounts.end();
         ++it) {
      int candidatePosition = it->first;
      int support = it->second;

      if (support < minSupport) {
        continue;
      }

      int mismatches = countMismatches(currentRead, reference, candidatePosition);

      if (mismatches <= maxMismatches) {
        MappingResult result;
        result.position = candidatePosition;
        result.isReverse = isReverse;
        result.mismatches = mismatches;
        result.matchedKmers = support;
        result.totalKmers = static_cast<int>(kmers.size());
        result.score = calculateMappingScore(support,
                                             static_cast<int>(kmers.size()),
                                             mismatches);
        result.cigar = generateCigar(currentRead, reference, candidatePosition);

        results.push_back(result);
      }
    }
  }

  sort(results.begin(), results.end(),
       [](const MappingResult &a, const MappingResult &b) {
         return a.score > b.score;
       });

  return results;
}