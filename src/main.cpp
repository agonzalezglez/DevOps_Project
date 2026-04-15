/**
 * @file main.cpp
 * @brief Entry point of the mapping program.
 * @author Alondra GONZALEZ
 * @date 2026
 *
 * This program performs a read mapping workflow:
 *
 * 1. read the reference genome from a FASTA file
 * 2. build an index on the reference using a suffix array
 * 3. detect and read input sequence files (FASTA or FASTQ)
 * 4. map each read against the indexed reference
 * 5. write mapping results to a TSV output file
 *
 */

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

#include "biological_sequence.h"
#include "file_reader.h"
#include "fasta_file_reader.h"
#include "fastq_file_reader.h"
#include "reference_index.h"
#include "mapper.h"

namespace fs = std::filesystem;

using namespace std;

/**
 * @brief Print program usage.
 *
 * Expected command-line format:
 *   program <reference.fasta> <input_directory/file> <output.tsv> [k=9] [max_mismatches=4]
 *
 * @param progname Program name
 */
static void printUsage(const string &progname) {
  cerr << "Usage: " << progname
       << " <reference.fasta> <input_directory> <output.tsv> [k=9] [max_mismatches=4]"
       << endl;
}

/**
 * @brief Check whether a path ends with one of the expected extensions.
 *
 * This helper is used to decide whether a file should be treated as
 * a candidate sequence file.
 *
 * Accepted extensions include:
 * - FASTA-like: .fasta, .fa, .fna
 * - FASTQ-like: .fastq, .fq
 *
 * @param path Input file path
 * @param extensions Allowed extensions
 * @return true if the path matches one of the extensions
 * @return false otherwise
 */
static bool hasExtension(const string &path, const vector<string> &extensions) {
  for (size_t i = 0; i < extensions.size(); ++i) {
    if (path.size() >= extensions[i].size() &&
        path.substr(path.size() - extensions[i].size()) == extensions[i]) {
      return true;
    }
  }

  return false;
}

/**
 * @brief List sequence files from a path.
 *
 * This helper accepts either:
 * - one regular file
 * - one directory containing multiple files
 *
 * If the input path is a file, it is returned only if its extension looks
 * like a supported sequence format.
 *
 * If the input path is a directory, all matching files are returned.
 *
 * @param path Input path
 * @return List of candidate sequence files
 */
static vector<string> listSequenceFiles(const string &path) {
  vector<string> files;

  if (!fs::exists(path)) {
    return files;
  }

  if (fs::is_regular_file(path)) {
    if (hasExtension(path, {".fasta", ".fa", ".fna", ".fastq", ".fq"})) {
      files.push_back(path);
    }
    return files;
  }

  if (fs::is_directory(path)) {
    for (fs::directory_iterator it(path); it != fs::directory_iterator(); ++it) {
      string current = it->path().string();

      if (fs::is_regular_file(it->path()) &&
          hasExtension(current, {".fasta", ".fa", ".fna", ".fastq", ".fq"})) {
        files.push_back(current);
      }
    }
  }

  return files;
}

/**
 * @brief Ensure that the output directory exists.
 *
 * If the parent directory of the output file does not exist,
 * this method attempts to create it.
 *
 * @param output_file Output file path
 * @return true if the directory exists or was created successfully
 * @return false otherwise
 */
static bool ensureOutputDirectory(const string &output_file) {
  fs::path p(output_file);
  fs::path dir = p.parent_path();

  if (dir.empty()) {
    return true;
  }

  if (!fs::exists(dir)) {
    return fs::create_directories(dir);
  }

  return true;
}

/**
 * @brief Main function of the mapping program.
 *
 * Pipeline:
 *
 * 1. parse command-line arguments
 * 2. open and read the reference FASTA file
 * 3. build a ReferenceIndex on the reference sequence
 * 4. detect input sequence files
 * 5. open an output TSV file
 * 6. for each input file:
 *    - detect whether it is FASTA or FASTQ
 *    - read sequences one by one
 *    - map each sequence using the mapper
 *    - write mapping results to the output file
 *
 * Result format:
 * The output is written as a TSV table containing:
 * - input filename
 * - read description
 * - candidate position
 * - strand
 * - mismatch count
 * - score
 * - CIGAR string
 * - matched k-mers
 * - total k-mers
 *
 * If no mapping candidate is accepted for one read, the program writes
 * a fallback line with position = -1 and empty mapping information.
 *
 * @param argc Number of command-line arguments
 * @param argv Argument values
 * @return Exit code
 */
int main(int argc, char **argv) {
  /**
   * Step 1: parse command-line arguments.
   */
  if (argc < 4 || argc > 6) {
    printUsage(argv[0]);
    return 1;
  }

  string reference_file = argv[1];
  string input_directory = argv[2];
  string output_file = argv[3];

  /**
   * Default mapping parameters.
   *
   * These values can be overridden by optional command-line arguments.
   */
  int kmer_size = 9;
  int max_mismatches = 4;

  if (argc >= 5) {
    kmer_size = atoi(argv[4]);
    if (kmer_size <= 0) {
      cerr << "Error: k must be > 0" << endl;
      return 1;
    }
  }

  if (argc == 6) {
    max_mismatches = atoi(argv[5]);
    if (max_mismatches < 0) {
      cerr << "Error: max_mismatches must be >= 0" << endl;
      return 1;
    }
  }

  /**
   * Step 2: ensure that the output directory exists.
   */
  if (!ensureOutputDirectory(output_file)) {
    cerr << "Error: unable to create output directory for '"
         << output_file << "'" << endl;
    return 2;
  }

  /**
   * Step 3: read the reference genome.
   *
   * The program expects the reference file to be FASTA.
   * Only the first sequence is currently read from the file.
   */
  FastaFileReader ref_reader(true);
  if (!ref_reader.open(reference_file, BiologicalSequence::DNA)) {
    cerr << "Error: unable to open reference FASTA file '"
         << reference_file << "'" << endl;
    return 3;
  }

  BiologicalSequence reference = ref_reader.nextSequence();
  if (reference.getSequence().empty()) {
    cerr << "Error: no reference sequence found in '"
         << reference_file << "'" << endl;
    return 4;
  }

  /**
   * Step 4: build the reference index.
   *
   * The reference is indexed once and reused for all reads.
   */
  ReferenceIndex index(reference.getSequence(), kmer_size);

  /**
   * Step 5: collect input sequence files.
   */
  vector<string> input_files = listSequenceFiles(input_directory);
  if (input_files.empty()) {
    cerr << "Error: no sequence files found in directory '"
         << input_directory << "'" << endl;
    return 5;
  }

  /**
   * Step 6: open output result file.
   */
  ofstream out(output_file.c_str());
  if (!out.is_open()) {
    cerr << "Error: unable to open output file '"
         << output_file << "'" << endl;
    return 6;
  }

  /**
   * Write TSV header.
   */
  out << "file\tread_description\tposition\tstrand\tmismatches\tscore\tcigar\tmatched_kmers\ttotal_kmers\n";

  /**
   * Step 7: process each input sequence file.
   */
  for (size_t f = 0; f < input_files.size(); ++f) {
    const string &current_file = input_files[f];

    /**
     * A generic FileReader pointer is used here so that the same code
     * can work with either FASTA or FASTQ readers.
     *
     * This is an example of polymorphism.
     */
    FileReader *reader = 0;

    /**
     * Try FASTA first.
     */
    FastaFileReader *fasta_reader = new FastaFileReader(true);
    if (fasta_reader->open(current_file, BiologicalSequence::DNA) &&
        fasta_reader->isValidFormat()) {
      reader = fasta_reader;
    } else {
      delete fasta_reader;

      /**
       * If FASTA detection fails, try FASTQ.
       */
      FastqFileReader *fastq_reader = new FastqFileReader(true);
      if (fastq_reader->open(current_file, BiologicalSequence::DNA) &&
          fastq_reader->isValidFormat()) {
        reader = fastq_reader;
      } else {
        delete fastq_reader;
        reader = 0;
      }
    }

    /**
     * Skip unsupported files.
     */
    if (!reader) {
      cerr << "[WARNING] file skipped (unknown format): "
           << current_file << endl;
      continue;
    }

    /**
     * Step 8: read and map sequences one by one.
     */
    while (true) {
      BiologicalSequence read = reader->nextSequence();

      /**
       * An empty sequence means there is nothing left to read.
       */
      if (read.getSequence().empty()) {
        break;
      }

      /**
       * Map current read against the indexed reference.
       */
      vector<Mapper::MappingResult> results =
        Mapper::mapRead(read, index, max_mismatches);

      /**
       * If no candidate is accepted, write a fallback line.
       */
      if (results.empty()) {
        out << current_file << "\t"
            << read.getDescription() << "\t"
            << -1 << "\t"
            << "*" << "\t"
            << -1 << "\t"
            << 0.0 << "\t"
            << "*" << "\t"
            << 0 << "\t"
            << 0 << "\n";
      } else {
        /**
         * Otherwise, write all accepted mapping candidates.
         *
         * Keeping all candidates is useful in this educational version
         * because it allows inspection of ambiguous mappings.
         */
        for (size_t i = 0; i < results.size(); ++i) {
          const Mapper::MappingResult &r = results[i];

          out << current_file << "\t"
              << read.getDescription() << "\t"
              << r.position << "\t"
              << (r.isReverse ? "-" : "+") << "\t"
              << r.mismatches << "\t"
              << r.score << "\t"
              << r.cigar << "\t"
              << r.matchedKmers << "\t"
              << r.totalKmers << "\n";
        }
      }
    }

    /**
     * Release reader object.
     */
    delete reader;
  }

  /**
   * Step 9: report completion.
   */
  cout << "Mapping finished. Results written to '"
       << output_file << "'" << endl;

  return 0;
}