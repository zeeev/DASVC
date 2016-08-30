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

    float pctA;
    int mbA;

    al.GetTag<float>("PI", pctA);
    al.GetTag<int>("MB", mbA   );

    for(std::vector< CigarOp >::iterator it = al.CigarData.begin();
        it != al.CigarData.end(); it++){

        if(it->Length > 20){

            if(it->Type == 'I'){
                std::cout << refName
                          << "\t" << rPos
                          << "\t" << rPos
                          << "\t" << it->Length
                          << "\t" << pctA
                          << "\t" << mbA
                          << "\t" << al.Name
                          << "\t" << "INS:INTERNAL"
                          << "\t" << al.QueryBases.substr(qPos, it->Length)
                          << std::endl;
            }
            if(it->Type == 'D'){
                std::cout << refName
                          << "\t" << rPos
                          << "\t" << (rPos + it->Length)
                          << "\t" << it->Length
                          << "\t" << pctA
                          << "\t" << mbA
                          << "\t" << al.Name
                          << "\t" << "DEL:INTERNAL"
                          << "\t" << targetFa.getSubSequence(refName,
                                                             rPos, it->Length)
                          << std::endl;
            }
        }
        advanceQuery(it->Type,     it->Length, &qPos);
        advanceReference(it->Type, it->Length, &rPos);
    }
}
//------------------------------------------------------------------------------
bool largeDeletionPrint(std::list<BamAlignment> & twoReads,
                        FastaReference & targetFasta,
                        std::string & refName ){

    if(twoReads.front().GetEndPosition() == twoReads.back().Position){
        return false;
    }
    if(twoReads.front().RefID != twoReads.back().RefID){
        return false;
    }

    /* start and end of the SV relative to query */

    int qStart ;
    int qEnd   ;

    int alA ;
    int alB ;

    float pctA;
    float pctB;

    int mbA;
    int mbB;


    twoReads.front().GetTag<int>("QE", qStart);
    twoReads.back().GetTag<int>("QS", qEnd);

    twoReads.front().GetTag<int>("AI", alA);
    twoReads.back().GetTag<int>("AI", alB);

    twoReads.front().GetTag<float>("PI", pctA);
    twoReads.back().GetTag<float>("PI", pctB);

    twoReads.front().GetTag<int>("MB", mbA);
    twoReads.back().GetTag<int>("MB", mbB);


    if(abs(alA - alB) > 1){
        return false;
    }

    int deletionL = abs( twoReads.back().Position -
                         twoReads.front().GetEndPosition());

    if(deletionL < 20){
        return false;
    }

    std::cout << refName
              << "\t" << twoReads.front().GetEndPosition()
              << "\t" << twoReads.back().Position
              << "\t" << deletionL
              << "\t" << pctA << "," << pctB
              << "\t" << mbA << "," << mbB
              << "\t" << twoReads.front().Name
              << "\t" << "DEL:BETWEEN"
              << "\t" << targetFasta.getSubSequence(refName,
                                                   twoReads.front().GetEndPosition(),
                                                    deletionL) << std::endl;


    return true;

}


//------------------------------------------------------------------------------


bool largeInsertionPrint(std::list<BamAlignment> & twoReads,
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

    if( twoReads.front().CigarData.back().Type != 'H' &&
        twoReads.back().CigarData.front().Type != 'H'){
        return false;
    }

    /* start and end of the SV relative to query */

    int qStart ;
    int qEnd   ;

    int alA ;
    int alB ;

    float pctA;
    float pctB;

    int mbA;
    int mbB;


    twoReads.front().GetTag<int>("QE", qStart);
    twoReads.back().GetTag<int>("QS", qEnd);

    twoReads.front().GetTag<int>("AI", alA);
    twoReads.back().GetTag<int>("AI", alB);

    twoReads.front().GetTag<float>("PI", pctA);
    twoReads.back().GetTag<float>("PI", pctB);

    twoReads.front().GetTag<int>("MB", mbA);
    twoReads.back().GetTag<int>("MB", mbB);


    /* next alignment block needs to be the next query block.
    */


    if(abs(alA - alB) > 1){
        return false;
    }


    int insertionL = qEnd - qStart;

    if(insertionL < 20){
        return false;
    }

    std::cout << refName
              << "\t" << twoReads.front().GetEndPosition()
              << "\t" << twoReads.front().GetEndPosition()
              << "\t" << insertionL
              << "\t" << pctA << "," << pctB
              << "\t" << mbA << "," << mbB
              << "\t" << twoReads.front().Name
              << "\t" << "INS:BETWEEN"
              << "\t" << queryFasta.getSubSequence(twoReads.front().Name,
                                                   qStart,
                                                   insertionL)

              << std::endl;

    return true;
}
//------------------------------- SUBROUTINE --------------------------------

int varcall(std::string & bname,   /* bam name */
            std::string & tf   ,   /* target fasta */
            std::string & qf    )  /* query fasta */
{

    std::cout << "#target_name\ttarget_pos\ttarget_end\tsv_length\tper_id\tmatching_bases\tquery_name\tsequence" << std::endl;

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

        if(!al.HasTag("AI")){
            EH.croak(EH.BAM_NOT_ANNOTATED, true);
        }

        if(readBuffer.size() == 2){
            largeInsertionPrint(readBuffer, queryFasta,
                                rv[al.RefID].RefName  );
            largeDeletionPrint(readBuffer, targetFasta,
                               rv[al.RefID].RefName  );


            readBuffer.pop_front();
        }

        readBuffer.push_back(al);
    }

    br.Close();

    return 0;
}
