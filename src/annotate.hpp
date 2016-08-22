#ifndef ANNOTATE_H
#define ANNOTATE_H

#include <list>
#include <string>
#include <iostream>
#include <algorithm>

/* bamtools */
#include "api/BamWriter.h"
#include "api/BamMultiReader.h"

/* my code */
#include "errors.hpp"
#include "cigar.hpp"
#include "chain.hpp"

using namespace BamTools;

int annotate(std::string, std::string);

#endif
