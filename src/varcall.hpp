#ifndef VARCALL_H
#define VARCALL_H


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <list>
#include <cmath>
#include "split.hpp"
#include "cigar.hpp"
#include "errors.hpp"

/* bamtools */
#include "api/BamWriter.h"
#include "api/BamMultiReader.h"

/* fasta hack */
#include "Fasta.h"

//------------------------------------------------------------------------------
void alignmentInternalPrint(BamAlignment   &,
                            FastaReference &,
                            std::string    & );

bool betweenAlignmentPrint(std::list<BamAlignment> &,
                           FastaReference          &,
                           std::string             & );

int varcall(std::string &, std::string &, std::string &);

#endif
