#include "annotate.hpp"

bool qStartSort(const BamAlignment L,
                const BamAlignment R ){

    int  lp;
    int  rp;

    L.GetTag<int>( "QS", lp );
    R.GetTag<int>( "QS", rp );

    if(lp <= rp){
        return true;
    }

    return false;
}


//------------------------------- SUBROUTINE --------------------------------
/*
 Function input  : list of BamAlignments
 Function does   : sorts and orients blocks by query position
 Function returns: error code
 Function details: Sorts alignments by first match position then adds
                   the new optional tag.

*/

int processBlock(std::list< BamAlignment > & readBuffer,
                 BamWriter & writer, int blockId)
{

    std::vector<BamAlignment> reads;

    for(std::list<BamAlignment>::iterator it = readBuffer.begin();
        it != readBuffer.end(); it++){

        long int queryStart = 0;

        for(std::vector<CigarOp>::iterator iz = it->CigarData.begin() ;
            iz != it->CigarData.end() ; iz++){
            /* first matching query position */
            advanceQuery(iz->Type, iz->Length, &queryStart, true);
            if( (iz->Type != 'H') || (iz->Type != 'S') ){
                break;
            }
        }

        long int queryEnd   = 0;
        int seen            = 0;

        for(std::vector<CigarOp>::iterator iz = it->CigarData.begin() ;
            iz != it->CigarData.end() ; iz++){
            /* first matching query position */

            if( (iz->Type == 'H') || (iz->Type == 'S') ){
                seen += 1;
            }
            if(seen > 1){
                break;
            }
            advanceQuery(iz->Type, iz->Length, &queryEnd, true);
        }

        double pctID = percentID(it->CigarData, it->Length);

        it->AddTag<int>( "QS", "i", queryStart      );
        it->AddTag<int>( "QE", "i", queryEnd        );
        it->AddTag<int>( "QL", "i", (queryEnd - queryStart));
        it->AddTag<float>( "PI", "f", pctID);
        it->AddTag<int>( "BI", "i", blockId);
        it->AddTag<int>( "NB", "i", readBuffer.size());

        reads.push_back(*it);
    }

    sort(reads.begin(), reads.end(),  qStartSort);

    for(std::list<BamAlignment>::iterator it = readBuffer.begin();
        it != readBuffer.end(); it++){
        writer.SaveAlignment(*it);
    }

    return 0;
}


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
    BamReader reader;

    errorHandler EH;

    if(!reader.Open(bfName)){
        EH.croak(EH.COULD_NOT_OPEN_BAM, true);
    }

    const SamHeader header     = reader.GetHeader();
    const RefVector references = reader.GetReferenceData();

    BamWriter writer;
    if ( !writer.Open("test.out.bam", header, references) ) {
        EH.croak(EH.COULD_NOT_OPEN_BAM, true);
    }


    EH.printInfo("Starting to annotate alignment blocks.");

    BamAlignment al;

    if(!reader.GetNextAlignment(al)){
        EH.croak(EH.NO_READS, true);
    }

    std::list<BamAlignment> readBuffer;

    int blockId = 0;

    while(reader.GetNextAlignment(al)){
        if(readBuffer.empty()){
            readBuffer.push_back(al);
            continue;
        }
        else{
            if(readBuffer.back().Name != al.Name){
                blockId += 1;
                processBlock(readBuffer, writer, blockId);
                readBuffer.clear()     ;
                readBuffer.push_back(al);
            }
            else{
                 readBuffer.push_back(al);
            }
        }
    }

    reader.Close();
    writer.Close();

    return 0;
}
