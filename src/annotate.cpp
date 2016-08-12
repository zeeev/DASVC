#include "errors.hpp"
#include <list>
#include <string>

/* bamtools */
#include "api/BamWriter.h"
#include "api/BamMultiReader.h"

using namespace BamTools;

//------------------------------- SUBROUTINE --------------------------------
/*
 Function input  : bam file input name
 Function does   : Opens a bam and adds an optional field
 Function returns: error code
 Function details: Adds a tag that describes the query position order
                   relative to the reference.

*/

int annotate(std::string bfName)
{
    BamReader br;

    errorHandler EH;

    if(!br.Open(bfName)){
        EH.croak(EH.COULD_NOT_OPEN_BAM, true);
    }
    if(!br.LocateIndex()){
        EH.croak(EH.COULD_NOT_OPEN_BAM_INDEX, true);
    }

    BamAlignment al;

    std::list<BamAlignment> readBuffer;

    if(!br.GetNextAlignment(al)){
        EH.croak(EH.NO_READS, true);
    }

    while(br.GetNextAlignment(al)){

    }

    return 0;
}
