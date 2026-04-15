/**
 * @file encoded_sequence.h
 * @brief Compact encoding of a nucleotide sequence.
 *
 * This class stores DNA sequences using 2 bits per nucleotide.
 * It allows storing 4 nucleotides in 1 byte.
 *
 * Note:
 * This class is not currently used in the mapping pipeline.
 * It represents a memory optimization for large genomes.
 */

#ifndef ENCODED_SEQUENCE_H
#define ENCODED_SEQUENCE_H

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Compact DNA sequence encoding.
 */
class EncodedSequence {
private:
  /**
   * @brief Encoded byte storage.
   */
  std::vector<unsigned char> _data;

  /**
   * @brief Number of nucleotides stored.
   */
  size_t _length;

  /**
   * @brief Encode a nucleotide into 2 bits.
   *
   * @param c Input nucleotide
   * @return Encoded value
   */
  unsigned char encodeNucleotide(char c) const;

  /**
   * @brief Decode a 2-bit code into a nucleotide.
   *
   * @param code Encoded value
   * @return Decoded nucleotide
   */
  char decodeNucleotide(unsigned char code) const;

public:
  /**
   * @brief Default constructor.
   */
  EncodedSequence();

  /**
   * @brief Construct and encode a sequence.
   *
   * @param sequence Input sequence
   */
  EncodedSequence(const std::string &sequence);

  /**
   * @brief Encode a sequence into compact format.
   *
   * @param sequence Input sequence
   */
  void encode(const std::string &sequence);

  /**
   * @brief Decode the stored sequence.
   *
   * @return Decoded sequence
   */
  std::string decode() const;

  /**
   * @brief Return sequence length.
   *
   * @return Number of nucleotides
   */
  size_t length() const;

  /**
   * @brief Check whether the encoded sequence is empty.
   *
   * @return true if empty
   * @return false otherwise
   */
  bool empty() const;

  /**
   * @brief Access a nucleotide at a given position.
   *
   * @param pos Position to read
   * @return Nucleotide at that position
   */
  char at(size_t pos) const;

  /**
   * @brief Modify a nucleotide at a given position.
   *
   * @param pos Position to modify
   * @param c New nucleotide
   */
  void set(size_t pos, char c);

  /**
   * @brief Print decoded sequence.
   *
   * @param os Output stream
   */
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<<(std::ostream &os, const EncodedSequence &seq) {
  seq.print(os);
  return os;
}

#endif