#include "annotate.hpp"

bool qStartSort(const BamAlignment L,
                const BamAlignment R ){

    int  lp;
    int  rp;

    if(!L.GetTag<int>( "QS", lp )){
        exit(1);
    }
    if(!R.GetTag<int>( "QS", rp )){
        exit(1);
    }

    if(lp < rp){
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

    int totalAlignedBases = 0;

    std::vector<BamAlignment> reads;

    for(std::list<BamAlignment>::iterator it = readBuffer.begin();
        it != readBuffer.end(); it++){

        long int queryStart = 0;

        for(std::vector<CigarOp>::iterator iz = it->CigarData.begin() ;
            iz != it->CigarData.end() ; iz++){
            /* first matching query position */
            if( (iz->Type != 'H') && (iz->Type != 'S') ){
                break;
            }
            advanceQuery(iz->Type, iz->Length, &queryStart, true);
        }

        long int queryEnd   = 0;
        int seen            = 0;

        /* if alignment starts without clip bump the stop condition */
        if(it->CigarData.front().Type != 'H'
           && it->CigarData.front().Type != 'S'){
            seen += 1;
        }


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

        int matchingBases = 0;

        double pctID = percentID(it->CigarData, it->Length, &matchingBases);

        totalAlignedBases += matchingBases;

        /* position where the alignment starts matching */

        it->AddTag<int>( "QS", "i", queryStart      );

        /* position in the aligment where it stops matching */

        it->AddTag<int>( "QE", "i", queryEnd        );

        /* lenght of the alignment block */

        it->AddTag<int>( "QL", "i", (queryEnd - queryStart));

        /* number of matching bases M= */

        it->AddTag<int>( "MB", "i", matchingBases);

        /* percent ID */

        it->AddTag<float>( "PI", "f", pctID);

        /* alignment group ; just another way of saying query name */

        it->AddTag<int>( "BI", "i", blockId);

        reads.push_back(*it);
    }


    sort(reads.begin(), reads.end(),  qStartSort);

    int contigOrder = 0;

    /* contig order information */

    for(int i = 0; i < reads.size(); i++){

        contigOrder++;

        /* alignment order for after the sort */
        reads[i].AddTag<int>( "AI", "i", contigOrder      );
        reads[i].AddTag<int>( "NB", "i", readBuffer.size());
        reads[i].AddTag<int>( "TM", "i", totalAlignedBases);

        std::stringstream finalAnno;
        int tmpTag;
        if(reads.size() == 1){
            finalAnno << ".:.:.";
        }

        else if(i == 0){
            finalAnno << ".:.:";
            reads[i+1].GetTag<int>("QS", tmpTag);
            finalAnno << tmpTag << ",";
            reads[i+1].GetTag<int>("QE", tmpTag);
            finalAnno << tmpTag;
            std::string strand = "+";
            if(reads[i+1].IsReverseStrand()){
                strand = "-";
            }
            finalAnno << strand;

        }
        else if(i == readBuffer.size() - 1){
            reads[i-1].GetTag<int>("QS", tmpTag);
            finalAnno << tmpTag << ",";
            reads[i-1].GetTag<int>("QE", tmpTag);
            finalAnno << tmpTag;
            std::string strand ="+";
            if(reads[i-1].IsReverseStrand()){
                strand = "-";
            }
            finalAnno << strand;
            finalAnno << ":.:.";
        }
        else{
            reads[i-1].GetTag<int>("QS", tmpTag);
            finalAnno << tmpTag << ",";
            reads[i-1].GetTag<int>("QE", tmpTag);
            finalAnno << tmpTag;
            std::string strand ="+";
            if(reads[i-1].IsReverseStrand()){
                strand = "-";
            }
            finalAnno << strand << ":.:";
            reads[i+1].GetTag<int>("QS", tmpTag);
            finalAnno << tmpTag << ",";
            reads[i+1].GetTag<int>("QE", tmpTag);
            finalAnno << tmpTag;
            strand = "+";
            if(reads[i+1].IsReverseStrand()){
                strand = "-";
            }
            finalAnno << strand;

        }
        reads[i].AddTag<std::string>("FA", "Z", finalAnno.str());
        writer.SaveAlignment(reads[i]);
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
    int numberOfAlignments = 0;

    while(reader.GetNextAlignment(al)){

        numberOfAlignments+=1;

        if((numberOfAlignments % 1000) == 0 ){
            std::stringstream ss;
            ss << "Number of contigs / alignments processed: " << blockId << " " << numberOfAlignments;
            EH.printInfo(ss.str());
        }


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

    blockId += 1;
    processBlock(readBuffer, writer, blockId);

    reader.Close();
    writer.Close();

    return 0;
}
