######################################
# Makefile written by Zev Kronenberg #
#     zev.kronenberg@gmail.com       #
######################################

CC=gcc
CXX=g++
CFLAGS= -Wall -DVERSION=\"$(GIT_VERSION)\" -std=c++0x -Wno-sign-compare -g
INCLUDE=-I src -I/net/gs/vol3/software/modules-sw/bamtools/2.2.3/Linux/RHEL6/x86_64/include
LIBS=-L/net/gs/vol3/software/modules-sw/bamtools/2.2.3/Linux/RHEL6/x86_64/lib
LINKER=-lbamtools


CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix src/obj/,$(notdir $(CPP_FILES:.cpp=.o)))

all: createBin bin/contigSV

createBin:
	-mkdir bin

src/obj/%.o: src/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) $(LIBS) -c -o $@ $<

bin/contigSV: $(OBJ_FILES)
	 $(CXX) $(CFLAGS) $(INCLUDE) $(OBJ_FILES) -o bin/contigSV $(LIBS) $(LINKER)
clean:
	rm -rf bin && rm -rf src/obj/*
