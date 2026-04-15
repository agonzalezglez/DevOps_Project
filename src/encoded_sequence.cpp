/** 
 * @file encoded_sequence.cpp
 * @brief Implementation of the EncodedSequence class.
 * @author Alondra GONZALEZ
 * @date 2026
 */

#include "encoded_sequence.h"

#include <cctype>
#include <stdexcept>

using namespace std;

EncodedSequence::EncodedSequence():
  _data(),
  _length(0) {
}

EncodedSequence::EncodedSequence(const string &sequence):
  _data(),
  _length(0) {
  encode(sequence);
}

unsigned char EncodedSequence::encodeNucleotide(char c) const {
  switch (toupper(static_cast<unsigned char>(c))) {
  case 'A':
    return 0; // 00
  case 'C':
    return 1; // 01
  case 'G':
    return 2; // 10
  case 'T':
  case 'U':
    return 3; // 11
  default:

    return 0;
  }
}

char EncodedSequence::decodeNucleotide(unsigned char code) const {
  switch (code & 0x03) {
  case 0:
    return 'A';
  case 1:
    return 'C';
  case 2:
    return 'G';
  case 3:
    return 'T';
  default:
    return 'A';
  }
}

void EncodedSequence::encode(const string &sequence) {
  _length = sequence.length();
  _data.clear();

  if (_length == 0) {
    return;
  }

  size_t nb_bytes = (_length + 3) / 4;
  _data.assign(nb_bytes, 0);

  for (size_t i = 0; i < _length; ++i) {
    unsigned char code = encodeNucleotide(sequence[i]);

    size_t byte_index = i / 4;
    size_t offset = i % 4;
    size_t shift = 6 - 2 * offset;

    _data[byte_index] |= static_cast<unsigned char>(code << shift);
  }
}

string EncodedSequence::decode() const {
  string sequence;
  sequence.reserve(_length);

  for (size_t i = 0; i < _length; ++i) {
    sequence.push_back(at(i));
  }

  return sequence;
}

size_t EncodedSequence::length() const {
  return _length;
}

bool EncodedSequence::empty() const {
  return (_length == 0);
}

char EncodedSequence::at(size_t pos) const {
  if (pos >= _length) {
    throw out_of_range("EncodedSequence::at(): position out of range");
  }

  size_t byte_index = pos / 4;
  size_t offset = pos % 4;
  size_t shift = 6 - 2 * offset;

  unsigned char code = static_cast<unsigned char>((_data[byte_index] >> shift) & 0x03);
  return decodeNucleotide(code);
}

void EncodedSequence::set(size_t pos, char c) {
  if (pos >= _length) {
    throw out_of_range("EncodedSequence::set(): position out of range");
  }

  size_t byte_index = pos / 4;
  size_t offset = pos % 4;
  size_t shift = 6 - 2 * offset;

  unsigned char code = encodeNucleotide(c);

  unsigned char mask = static_cast<unsigned char>(~(0x03 << shift));
  _data[byte_index] &= mask;
  _data[byte_index] |= static_cast<unsigned char>(code << shift);
}

void EncodedSequence::print(ostream &os) const {
  os << decode();
}