
################### Compilation configuration ###################

CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -I./include
MSQ = @

BUILD_DIR = build
BIN_DIR = bin
DEPDIR = .dep

################### Programs to generate ###################

PROGS = $(BIN_DIR)/mapping $(BIN_DIR)/table_suffixes

################### Sources for mapping program ###################

mapping_SOURCES = \
	src/main.cpp \
	src/biological_sequence.cpp \
	src/file_reader.cpp \
	src/fasta_file_reader.cpp \
	src/fastq_file_reader.cpp \
	src/suffix_array.cpp \
	src/reference_index.cpp \
	src/mapper.cpp

mapping_OBJECTS = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(mapping_SOURCES))


################### Sources for suffix table program ###################

suffix_SOURCES = \
	src/table_suffixes.cpp \
	src/suffix_array.cpp

suffix_OBJECTS = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(suffix_SOURCES))


################### General targets ###################

.PHONY: all clean

all: $(DEPDIR) $(BUILD_DIR) $(BIN_DIR) $(PROGS)

$(DEPDIR):
	@echo "Creating directory $(DEPDIR)"
	$(MSQ)mkdir -p $(DEPDIR)

$(BUILD_DIR):
	@echo "Creating directory $(BUILD_DIR)"
	$(MSQ)mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@echo "Creating directory $(BIN_DIR)"
	$(MSQ)mkdir -p $(BIN_DIR)


################### Link edition ###################

$(BIN_DIR)/mapping: $(mapping_OBJECTS) | $(BIN_DIR)
	@echo "Linking $@"
	$(MSQ)$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Executable generated: $@"

$(BIN_DIR)/table_suffixes: $(suffix_OBJECTS) | $(BIN_DIR)
	@echo "Linking $@"
	$(MSQ)$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Executable generated: $@"

################### Object compilation ################### 

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR) $(DEPDIR)
	@echo "Compiling $<"
	$(MSQ)$(CXX) $(CXXFLAGS) -MMD -MP -MF $(DEPDIR)/$(subst /,_,$(@:.o=.d)) -c $< -o $@

-include $(wildcard $(DEPDIR)/*.d)


################### Cleaning  ###################

clean:
	@echo "Cleaning generated files"
	$(MSQ)rm -rf $(BUILD_DIR) $(BIN_DIR) $(DEPDIR) *~
	@echo "Cleaning done"