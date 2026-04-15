/**
 * @file biological_sequence.cpp
 * @brief Implementation of the BiologicalSequence class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This file implements the basic sequence container used throughout the project.
 *
 * - readers create BiologicalSequence objects from FASTA or FASTQ files
 * - the sequence is normalized and stored
 * - validation methods can then be applied independently of file format
 *
 */

#include "biological_sequence.h"

#include <cctype>
#include <cmath>
#include <stdexcept>

using namespace std;

/**
 * @brief Remove leading and trailing whitespace from a string.
 *
 * This helper is used mainly for descriptions, so that headers do not
 * keep unnecessary spaces at the beginning or end.
 *
 * @param s Input string
 * @return Trimmed string
 */
static string trim(const string &s) {
  size_t start = 0;
  size_t end = s.size();

  while (start < end && s[start] <= ' ') {
    ++start;
  }

  while (end > start && s[end - 1] <= ' ') {
    --end;
  }

  return s.substr(start, end - start);
}

/**
 * @brief Remove whitespace from a sequence and convert it to uppercase.
 *
 * This helper function gives the sequence a normalized internal form.
 *
 * Example:
 * - "a c g t" becomes "ACGT"
 * - lowercase letters become uppercase
 *
 * @param s Input sequence
 * @return Cleaned sequence
 */
static string cleanSequence(const string &s) {
  string result;

  for (size_t i = 0; i < s.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(s[i]);

    if (!isspace(c)) {
      result += static_cast<char>(toupper(c));
    }
  }

  return result;
}

/**
 * @brief Build an empty BiologicalSequence object.
 *
 * The object starts in a neutral state:
 * - empty description
 * - empty sequence
 * - empty quality
 * - UNKNOWN type
 */
BiologicalSequence::BiologicalSequence() {
  _description = "";
  _sequence = "";
  _quality = "";
  _type = UNKNOWN;
}

/**
 * @brief Build a BiologicalSequence object from given values.
 *
 * The constructor stores:
 * - description
 * - sequence
 * - quality
 *
 * Then it tries to assign the requested biological type.
 * If the type is incompatible with the sequence, the final type
 * becomes UNKNOWN.
 *
 * @param description Sequence description
 * @param sequence Biological sequence
 * @param quality Quality string
 * @param type Requested biological type
 */
BiologicalSequence::BiologicalSequence(const string &description,
                                       const string &sequence,
                                       const string &quality,
                                       Type type) {
  setDescription(description);
  setSequence(sequence);
  setQuality(quality);
  _type = UNKNOWN;

  if (!setType(type)) {
    _type = UNKNOWN;
  }
}

/**
 * @brief Reset the object to an empty state.
 *
 */
void BiologicalSequence::clear() {
  _description.clear();
  _sequence.clear();
  _quality.clear();
  _type = UNKNOWN;
}

/**
 * @brief Store a cleaned sequence description.
 *
 * Only leading and trailing spaces are removed here.
 *
 * @param description New description
 */
void BiologicalSequence::setDescription(const string &description) {
  _description = trim(description);
}

/**
 * @brief Return stored description.
 *
 * @return Description string
 */
string BiologicalSequence::getDescription() const {
  return _description;
}

/**
 * @brief Store a biological sequence.
 *
 * The sequence is normalized by:
 * - removing whitespace
 * - converting letters to uppercase
 *
 * If the current type becomes incompatible with the new sequence,
 * the type is reset to UNKNOWN.
 *
 * @param sequence New biological sequence
 */
void BiologicalSequence::setSequence(const string &sequence) {
  _sequence = cleanSequence(sequence);

  if ((_type != UNKNOWN) && !setType(_type)) {
    _type = UNKNOWN;
  }
}

/**
 * @brief Return stored sequence.
 *
 * @return Biological sequence string
 */
string BiologicalSequence::getSequence() const {
  return _sequence;
}

/**
 * @brief Store quality string.
 *
 * In the current implementation, the quality string is stored as is.
 * No length consistency check is enforced here.
 *
 * @param quality New quality string
 */
void BiologicalSequence::setQuality(const string &quality) {
  _quality = quality;
}

/**
 * @brief Return stored quality string.
 *
 * @return Quality string
 */
string BiologicalSequence::getQuality() const {
  return _quality;
}

/**
 * @brief Assign a biological type if compatible with the sequence.
 *
 * Validation is done by testing each sequence character against the
 * alphabet defined by the requested type.
 *
 * If type is UNKNOWN, the object is simply set to UNKNOWN.
 *
 * @param type Requested type
 * @return true if the assignment succeeds
 * @return false if the sequence contains incompatible symbols
 */
bool BiologicalSequence::setType(Type type) {
  if (type == UNKNOWN) {
    _type = UNKNOWN;
    return true;
  }

  for (size_t i = 0; i < _sequence.length(); ++i) {
    if (!isValid(_sequence[i], type)) {
      return false;
    }
  }

  _type = type;
  return true;
}

/**
 * @brief Return current biological type.
 *
 * @return Stored type
 */
BiologicalSequence::Type BiologicalSequence::getType() const {
  return _type;
}

/**
 * @brief Return sequence length.
 *
 * @return Number of symbols in the sequence
 */
size_t BiologicalSequence::length() const {
  return _sequence.length();
}

/**
 * @brief Check whether the sequence is empty.
 *
 * @return true if sequence string is empty
 * @return false otherwise
 */
bool BiologicalSequence::empty() const {
  return _sequence.empty();
}

/**
 * @brief Access one sequence character with bounds checking.
 *
 * This is a safe access method.
 *
 * @param pos Position to access
 * @return Character at the requested position
 * @throw std::out_of_range if pos is invalid
 */
char BiologicalSequence::at(size_t pos) const {
  if (pos >= _sequence.length()) {
    throw out_of_range("BiologicalSequence::at(): position out of range");
  }

  return _sequence[pos];
}

/**
 * @brief Access operator for sequence characters.
 *
 * This implementation forwards to at(), so it keeps bounds checking.
 *
 * @param pos Position to access
 * @return Character at the requested position
 */
char BiologicalSequence::operator[](size_t pos) const {
  return at(pos);
}

/**
 * @brief Extract a subsequence from the stored sequence.
 *
 * If the starting position is outside the sequence, an empty string
 * is returned.
 *
 * @param pos Start position
 * @param len Requested length
 * @return Extracted subsequence
 */
string BiologicalSequence::subsequence(size_t pos, size_t len) const {
  if (pos >= _sequence.length()) {
    return "";
  }

  return _sequence.substr(pos, len);
}

/**
 * @brief Estimate error probability from one quality symbol.
 *
 * This method assumes a Phred+33 quality encoding.
 *
 * Computation:
 * - Q = ASCII code - 33
 * - p = 10^(-Q / 10)
 *
 * If the requested position is outside the quality string,
 * the method returns -1.0.
 *
 * @param pos Position in the quality string
 * @return Error probability or -1.0 if unavailable
 */
double BiologicalSequence::getProbabilityOfError(size_t pos) const {
  if (pos >= _quality.length()) {
    return -1.0;
  }

  int q = static_cast<int>(_quality[pos]) - 33;
  return pow(10.0, -q / 10.0);
}

/**
 * @brief Check whether a character is valid for the current type.
 *
 * This method uses the stored biological type of the object.
 *
 * @param c Character to test
 * @return true if valid
 * @return false otherwise
 */
bool BiologicalSequence::isValid(char c) const {
  return isValid(c, _type);
}

/**
 * @brief Check whether a character is valid for a given biological type.
 *
 * This method dispatches the validation to the correct alphabet-specific
 * helper function.
 *
 * @param c Character to test
 * @param type Type used for validation
 * @return true if valid
 * @return false otherwise
 */
bool BiologicalSequence::isValid(char c, Type type) {
  switch (type) {
  case DNA:
    return isDNA(c);
  case EXTENDED_DNA:
    return isExtendedDNA(c);
  case RNA:
    return isRNA(c);
  case EXTENDED_RNA:
    return isExtendedRNA(c);
  case AMINO_ACID:
    return isAminoAcid(c);
  case UNKNOWN:
  default:
    return true;
  }
}

/**
 * @brief Check whether a character belongs to standard DNA alphabet.
 *
 * Accepted symbols:
 * - A
 * - C
 * - G
 * - T
 *
 * @param c Character to test
 * @return true if valid DNA symbol
 * @return false otherwise
 */
bool BiologicalSequence::isDNA(char c) {
  c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
  return (c == 'A') || (c == 'C') || (c == 'G') || (c == 'T');
}

/**
 * @brief Check whether a character belongs to extended DNA alphabet.
 *
 * Accepted symbols:
 * - A, C, G, T
 * - IUPAC ambiguity symbols
 * - '.' and '-' for gaps
 *
 * @param c Character to test
 * @return true if valid extended DNA symbol
 * @return false otherwise
 */
bool BiologicalSequence::isExtendedDNA(char c) {
  c = static_cast<char>(toupper(static_cast<unsigned char>(c)));

  return
    (c == 'A') || (c == 'C') || (c == 'G') || (c == 'T') ||
    (c == 'R') || (c == 'Y') || (c == 'W') || (c == 'S') ||
    (c == 'K') || (c == 'M') || (c == 'B') || (c == 'D') ||
    (c == 'H') || (c == 'V') || (c == 'N') ||
    (c == '-') || (c == '.');
}

/**
 * @brief Check whether a character belongs to standard RNA alphabet.
 *
 * Accepted symbols:
 * - A
 * - C
 * - G
 * - U
 *
 * @param c Character to test
 * @return true if valid RNA symbol
 * @return false otherwise
 */
bool BiologicalSequence::isRNA(char c) {
  c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
  return (c == 'A') || (c == 'C') || (c == 'G') || (c == 'U');
}

/**
 * @brief Check whether a character belongs to extended RNA alphabet.
 *
 * Accepted symbols:
 * - A, C, G, U
 * - IUPAC ambiguity symbols
 * - '.' and '-' for gaps
 *
 * @param c Character to test
 * @return true if valid extended RNA symbol
 * @return false otherwise
 */
bool BiologicalSequence::isExtendedRNA(char c) {
  c = static_cast<char>(toupper(static_cast<unsigned char>(c)));

  return
    (c == 'A') || (c == 'C') || (c == 'G') || (c == 'U') ||
    (c == 'R') || (c == 'Y') || (c == 'W') || (c == 'S') ||
    (c == 'K') || (c == 'M') || (c == 'B') || (c == 'D') ||
    (c == 'H') || (c == 'V') || (c == 'N') ||
    (c == '-') || (c == '.');
}

/**
 * @brief Check whether a character belongs to the amino acid alphabet.
 *
 * This implementation accepts one-letter amino acid codes.
 *
 * @param c Character to test
 * @return true if valid amino acid symbol
 * @return false otherwise
 */
bool BiologicalSequence::isAminoAcid(char c) {
  c = static_cast<char>(toupper(static_cast<unsigned char>(c)));

  return
    (c == 'A') || (c == 'C') || (c == 'D') || (c == 'E') ||
    (c == 'F') || (c == 'G') || (c == 'H') || (c == 'I') ||
    (c == 'K') || (c == 'L') || (c == 'M') || (c == 'N') ||
    (c == 'O') || (c == 'P') || (c == 'Q') || (c == 'R') ||
    (c == 'S') || (c == 'T') || (c == 'U') || (c == 'V') ||
    (c == 'W') || (c == 'Y');
}