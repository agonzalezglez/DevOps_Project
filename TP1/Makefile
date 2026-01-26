CXX = g++
PROG = read_file_fasta

.PHONY: all clean

all: $(PROG)

read_file_fasta: read_file_fasta.cpp
	$(CXX) $< -o $@

clean:
	rm -f "$(PROG)"
	rm -f *~           
