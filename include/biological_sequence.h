/**
 * @file biological_sequence.h
 * @brief Definition of the BiologicalSequence class.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This class represents a biological sequence together with its metadata.
 *
 * In this project, FASTA and FASTQ files are converted into objects of this
 * class so that the rest of the program can manipulate sequences in a common
 * way.
 *
 * A BiologicalSequence object may contain:
 * - a textual description
 * - a biological sequence
 * - an optional quality string
 * - a biological type
 *
 * This design keeps the parsing step separated from the mapping step.
 */

#ifndef BIOLOGICAL_SEQUENCE_H
#define BIOLOGICAL_SEQUENCE_H

#include <cstddef>
#include <string>

/**
 * @brief Representation of a biological sequence.
 *
 * This class is used as a container for sequence data.
 * It can represent:
 * - DNA
 * - extended DNA
 * - RNA
 * - extended RNA
 * - amino acid sequences
 *
 * The class also stores an optional quality string, which is useful
 * for FASTQ data.
 *
 * Main goals of this class:
 * - provide one common object for FASTA and FASTQ readers
 * - normalize input sequences
 * - offer basic validation methods
 */
class BiologicalSequence {
public:
  /**
   * @brief Possible biological sequence types.
   *
   * The type determines which alphabet is considered valid.
   */
  enum Type {
    UNKNOWN,       /**< Unknown or not yet validated type */
    DNA,           /**< Standard DNA alphabet: A, C, G, T */
    EXTENDED_DNA,  /**< Extended DNA alphabet with IUPAC ambiguity symbols */
    RNA,           /**< Standard RNA alphabet: A, C, G, U */
    EXTENDED_RNA,  /**< Extended RNA alphabet with IUPAC ambiguity symbols */
    AMINO_ACID     /**< Amino acid alphabet */
  };

private:
  /**
   * @brief Sequence description.
   *
   * This usually corresponds to the FASTA or FASTQ header line,
   * without the initial '>' or '@'.
   */
  std::string _description;

  /**
   * @brief Biological sequence string.
   *
   * The internal sequence is stored as text and is usually normalized
   * to uppercase with whitespace removed.
   */
  std::string _sequence;

  /**
   * @brief Quality string associated with the sequence.
   *
   * This field is mainly useful for FASTQ reads.
   * For FASTA sequences, it is empty.
   */
  std::string _quality;

  /**
   * @brief Biological type of the stored sequence.
   *
   * This type is used for sequence validation.
   */
  Type _type;

public:
  /**
   * @brief Default constructor.
   *
   * Builds an empty sequence object with UNKNOWN type.
   */
  BiologicalSequence();

  /**
   * @brief Constructor with parameters.
   *
   * The constructor stores the provided values and tries to assign
   * the requested biological type.
   *
   * If the sequence is not compatible with the requested type,
   * the type falls back to UNKNOWN.
   *
   * @param description Sequence description
   * @param sequence Biological sequence
   * @param quality Quality string
   * @param type Biological type
   */
  BiologicalSequence(const std::string &description,
                     const std::string &sequence,
                     const std::string &quality = "",
                     Type type = UNKNOWN);

  /**
   * @brief Reset the object to an empty state.
   *
   * This clears:
   * - description
   * - sequence
   * - quality
   * and resets the type to UNKNOWN.
   */
  void clear();

  /**
   * @brief Set sequence description.
   *
   * Leading and trailing whitespace are removed before storing.
   *
   * @param description New description
   */
  void setDescription(const std::string &description);

  /**
   * @brief Return sequence description.
   *
   * @return Stored description
   */
  std::string getDescription() const;

  /**
   * @brief Set biological sequence.
   *
   * The sequence is cleaned before storing:
   * - whitespace is removed
   * - letters are converted to uppercase
   *
   * If the current type becomes incompatible with the new sequence,
   * the type is reset to UNKNOWN.
   *
   * @param sequence New sequence
   */
  void setSequence(const std::string &sequence);

  /**
   * @brief Return biological sequence.
   *
   * @return Stored sequence
   */
  std::string getSequence() const;

  /**
   * @brief Set quality string.
   *
   * This method stores the quality string as provided.
   * It does not currently enforce consistency with sequence length.
   *
   * @param quality New quality string
   */
  void setQuality(const std::string &quality);

  /**
   * @brief Return quality string.
   *
   * @return Stored quality string
   */
  std::string getQuality() const;

  /**
   * @brief Set biological type if it is compatible with the sequence.
   *
   * Validation is performed character by character using the alphabet
   * associated with the requested type.
   *
   * @param type New biological type
   * @return true if the type is accepted
   * @return false if at least one sequence symbol is incompatible
   */
  bool setType(Type type);

  /**
   * @brief Return current biological type.
   *
   * @return Stored type
   */
  Type getType() const;

  /**
   * @brief Return sequence length.
   *
   * This length corresponds to the biological sequence only.
   *
   * @return Sequence length
   */
  size_t length() const;

  /**
   * @brief Check whether the sequence is empty.
   *
   * @return true if the sequence string is empty
   * @return false otherwise
   */
  bool empty() const;

  /**
   * @brief Access a character with bounds checking.
   *
   * @param pos Position in the sequence
   * @return Character at the requested position
   * @throw std::out_of_range if position is invalid
   */
  char at(size_t pos) const;

  /**
   * @brief Access operator.
   *
   * This operator behaves like at() in this implementation.
   *
   * @param pos Position in the sequence
   * @return Character at the requested position
   */
  char operator[](size_t pos) const;

  /**
   * @brief Extract a subsequence.
   *
   * If the starting position is outside the sequence, an empty string
   * is returned.
   *
   * @param pos Start position
   * @param len Requested length
   * @return Extracted subsequence
   */
  std::string subsequence(size_t pos, size_t len) const;

  /**
   * @brief Estimate error probability at one position from quality score.
   *
   * This method assumes a Phred+33 encoding:
   *   Q = ASCII - 33
   *   p = 10^(-Q/10)
   *
   * If the requested position is outside the quality string,
   * the method returns -1.
   *
   * @param pos Position in the quality string
   * @return Estimated probability of error, or -1 if unavailable
   */
  double getProbabilityOfError(size_t pos) const;

  /**
   * @brief Check whether a character is valid for the current type.
   *
   * @param c Character to test
   * @return true if valid
   * @return false otherwise
   */
  bool isValid(char c) const;

  /**
   * @brief Check whether a character is valid for a specific type.
   *
   * This is a generic dispatcher that forwards validation to the
   * appropriate alphabet-specific method.
   *
   * @param c Character to test
   * @param type Type used for validation
   * @return true if valid
   * @return false otherwise
   */
  static bool isValid(char c, Type type);

  /**
   * @brief Check whether a character belongs to the standard DNA alphabet.
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
  static bool isDNA(char c);

  /**
   * @brief Check whether a character belongs to the extended DNA alphabet.
   *
   * Accepted symbols include:
   * - standard DNA symbols
   * - IUPAC ambiguity symbols
   * - gap symbols '.' and '-'
   *
   * @param c Character to test
   * @return true if valid extended DNA symbol
   * @return false otherwise
   */
  static bool isExtendedDNA(char c);

  /**
   * @brief Check whether a character belongs to the standard RNA alphabet.
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
  static bool isRNA(char c);

  /**
   * @brief Check whether a character belongs to the extended RNA alphabet.
   *
   * Accepted symbols include:
   * - standard RNA symbols
   * - IUPAC ambiguity symbols
   * - gap symbols '.' and '-'
   *
   * @param c Character to test
   * @return true if valid extended RNA symbol
   * @return false otherwise
   */
  static bool isExtendedRNA(char c);

  /**
   * @brief Check whether a character belongs to the amino acid alphabet.
   *
   * This implementation accepts standard one-letter amino acid codes.
   *
   * @param c Character to test
   * @return true if valid amino acid symbol
   * @return false otherwise
   */
  static bool isAminoAcid(char c);
};

#endif