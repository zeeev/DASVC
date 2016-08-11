#ifndef CIGAR_H
#define CIGAR_H

#include <vector>
#include <sstream>
#include <string>
#include <stdint.h>
#include "api/BamMultiReader.h"

#define BAM_CIGAR_SHIFT 4
#define BAM_CIGAR_MASK  ((1 << BAM_CIGAR_SHIFT) - 1)

using namespace BamTools;

void   advanceReference(const char, const uint32_t, long int *);
void   advanceQuery(    const char, const uint32_t, long int *);
double percentID(   std::vector< CigarOp > & cigs, int32_t len);

#endif
