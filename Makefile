######################################
# Makefile written by Zev Kronenberg #
#     zev.kronenberg@gmail.com       #
######################################
#-------------------------------------------------------------------------------

CC=gcc
CXX=g++
CFLAGS= -Wall -DVERSION=\"$(GIT_VERSION)\" -std=c++0x
INCLUDE  = -I src -I fastahack -I bamtools/include
BAMTOOLS_LIB=bamtools/lib/libbamtools.a


CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix src/,$(notdir $(CPP_FILES:.cpp=.o)))
OBJ_FILES := $(OBJ_FILES) fastahack/disorder.o
OBJ_FILES := $(OBJ_FILES) fastahack/Fasta.o
#-------------------------------------------------------------------------------

all: createBin bin/contigSV

createBin:
	-mkdir bin

src/%.o: src/%.cpp $(BAMTOOLS_LIB)
	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<

bin/contigSV: $(OBJ_FILES) $(BAMTOOLS_LIB)
	 $(CXX) $(CFLAGS) $(INCLUDE) $(OBJ_FILES) $(BAMTOOLS_LIB) -o bin/contigSV -lm -lz

fastahack/%.o:
	cd fastahack && $(MAKE) ;

bamtools/lib/libbamtools.a:
	cd bamtools && mkdir -p build && cd build && cmake .. && $(MAKE)

clean:
	rm -rf bin && rm -rf src/*.o && cd fastahack && $(MAKE) clean

#-------------------------------------------------------------------------------