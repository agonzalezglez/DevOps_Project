CXX = g++
PROG = read_file_

.PHONY: all clean

all: $(PROG)

read_file_fasta: read_file_.cpp
	$(CXX) $< -o $@

clean:
	rm -f "$(PROG)"
	rm -f *~           
