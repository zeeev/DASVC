#ifndef ANNOTATE_H
#define ANNOTATE_H

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

/* bamtools */
#include "api/BamWriter.h"
#include "api/BamMultiReader.h"

/* my code and other local */
#include "errors.hpp"
#include "cigar.hpp"
#include "chain.hpp"
#include "split.hpp"

using namespace BamTools;

int annotate(std::string, std::string, std::string);

#endif
