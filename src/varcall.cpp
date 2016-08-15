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
void alignmentInternalPrint(BamAlignment & al,
                            FastaReference & targetFa,
                            std::string & refName )
{

    long int qPos = 0;
    long int rPos = al.Position;

    for(std::vector< CigarOp >::iterator it = al.CigarData.begin();
        it != al.CigarData.end(); it++){

        if(it->Type == 'I'){
            std::cout << "INSERTION"
                      << "\t" << refName
                      << "\t" << rPos
                      << "\t" << it->Length
                      << "\t" << al.QueryBases.substr(qPos, it->Length)
                      << std::endl;
        }
        if(it->Type == 'D'){
            std::cout << "DELETION"
                      << "\t" << refName
                      << "\t" << rPos
                      << "\t" << it->Length
                      << "\t" << targetFa.getSubSequence(refName,
                                                         rPos, it->Length)
                      << std::endl;
        }
        advanceQuery(it->Type,     it->Length, &qPos);
        advanceReference(it->Type, it->Length, &rPos);
    }
}
//------------------------------------------------------------------------------

bool betweenAlignmentPrint(std::list<BamAlignment> & twoReads,
                           FastaReference & queryFasta,
                           std::string & refName )
{
    if(twoReads.front().RefID != twoReads.back().RefID){
        return false;
    }
    /* deletions bodering and insertion will be a problem */
    if(twoReads.front().GetEndPosition() != twoReads.back().Position){
        return false;
    }
    if(twoReads.front().IsReverseStrand()
       != twoReads.back().IsReverseStrand()){
        return false;
    }
    if( twoReads.front().CigarData.back().Type != 'H' &&
        twoReads.back().CigarData.front().Type != 'H'){
        return false;
    }

    /* start and end of the SV relative to query */

    int qStart ;
    int qEnd   ;

    int alA ;
    int alB ;

    twoReads.front().GetTag<int>("QE", qStart);
    twoReads.back().GetTag<int>("QS", qEnd);

    twoReads.front().GetTag<int>("AI", alA);
    twoReads.back().GetTag<int>("AI", alB);

    /* next alignment block needs to be the next query block.
    */


    if((alA + 1) != alB){
        return false;
    }

    int insertionL = abs(qEnd - qStart);

    std::cout << "INS"
              << "\t" << refName
              << "\t" << twoReads.front().Name
              << "\t" << twoReads.front().GetEndPosition()
              << "\t" << insertionL
              << "\t" << queryFasta.getSubSequence(twoReads.front().Name,
                                                   qStart,
                                                   insertionL)

              << std::endl;

    return true;
}
//------------------------------- SUBROUTINE --------------------------------

bool varcall(std::string & bname,   /* bam name */
            std::string & tf   ,   /* target fasta */
            std::string & qf    )  /* query fasta */
{

    errorHandler EH;

    /* target is the ref genome */
    /* query is required for inter alignment insertion */

    FastaReference targetFasta ;
    FastaReference queryFasta  ;

    targetFasta.open(tf.c_str()) ;
    queryFasta.open( qf.c_str()) ;

    BamReader br;

    if(!br.Open(bname)){
        EH.croak(EH.COULD_NOT_OPEN_BAM, true);
    }
    if(!br.LocateIndex()){
        EH.croak(EH.COULD_NOT_OPEN_BAM_INDEX, true);
    }

    EH.printInfo("Starting variant calling");

    RefVector rv     = br.GetReferenceData();

    BamAlignment al;

    std::list<BamAlignment> readBuffer;

    while(br.GetNextAlignment(al)){
        alignmentInternalPrint(al, targetFasta, rv[al.RefID].RefName);

        if(!al.HasTag("FA")){
            EH.croak(EH.BAM_NOT_ANNOTATED, true);
        }

        if(readBuffer.size() == 2){
            betweenAlignmentPrint(readBuffer, queryFasta,
                                  rv[al.RefID].RefName);
            readBuffer.pop_front();
        }

        readBuffer.push_back(al);
    }

    betweenAlignmentPrint(readBuffer, queryFasta,
                      rv[al.RefID].RefName);

    br.Close();

    return true;
}
