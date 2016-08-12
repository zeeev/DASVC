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

    long int qPosH = 0;

    /* You need to know the query stop from the start (include leading clip)
       to the base before the hard clip at the end.
    */

    for(unsigned int i = 0 ; i < twoReads.front().CigarData.size() -1; i++){
        advanceQuery(twoReads.front().CigarData[i].Type,
                     twoReads.front().CigarData[i].Length, &qPosH, true);
    }

    long int insertionL = twoReads.back().CigarData.front().Length - qPosH ;

    if(insertionL < 0){
        std::cerr << "WARNING: decreasing query pos" << std::endl;
        return false;
    }

    std::cout << "INS"
              << "\t" << refName
              << "\t" << twoReads.front().Name
              << "\t" << twoReads.front().GetEndPosition()
              << "\t" << insertionL
              << "\t" << queryFasta.getSubSequence(twoReads.front().Name,
                                                   qPosH,
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

        if(readBuffer.size() == 2){
            betweenAlignmentPrint(readBuffer, queryFasta,
                                  rv[al.RefID].RefName);
            readBuffer.pop_front();
        }

        readBuffer.push_back(al);
    }

    betweenAlignmentPrint(readBuffer, queryFasta,
                      rv[al.RefID].RefName);

    return true;
}
