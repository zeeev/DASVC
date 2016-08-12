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

    EH.printInfo("Starting to annotate alignment blocks.");

    BamAlignment al;

    if(!br.GetNextAlignment(al)){
        EH.croak(EH.NO_READS, true);
    }

    std::list<BamAlignment> readBuffer;

    while(br.GetNextAlignment(al)){
        if(readBuffer.empty()){
            readBuffer.push_back(al);
            continue;
        }
        else{
            if(readBuffer.back().Name != al.Name){

                //orientBlocks();
                readBuffer.clear()     ;
                readBuffer.push_back(al);
            }
        }
    }

    return 0;
}
