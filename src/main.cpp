#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include "split.hpp"
#include "cigar.hpp"

/* bamtools */
#include "api/BamWriter.h"
#include "api/BamMultiReader.h"

/* fasta hack */
#include "Fasta.h"

using namespace BamTools;

//------------------------------------------------------------------------------
void readInternalPrint(BamAlignment & al,
                       FastaReference & targetFa,
                       std::string & refName )
{
    std::cerr << al.Name
              << "\t" << al.Position
              << "\t" << al.GetEndPosition()
              << "\t" << al.Length
              << "\t" << percentID(al.CigarData, al.Length) << std::endl;

    long int qPos = 0;
    long int rPos = al.Position;

    for(std::vector< CigarOp >::iterator it = al.CigarData.begin();
        it != al.CigarData.end(); it++){

        if(it->Type == 'I'){
            std::cout << "INSERTION"
                      << "\t" << rPos
                      << "\t" << al.QueryBases.substr(qPos, it->Length)
                      << std::endl;
        }
        if(it->Type == 'D'){
            std::cout << "DELETION"
                      << "\t" << rPos
                      << "\t" << targetFa.getSubSequence(refName,
                                                         rPos+1, it->Length)
                      << std::endl;
        }
        advanceQuery(it->Type,     it->Length, &qPos);
        advanceReference(it->Type, it->Length, &rPos);
    }
}

//------------------------------------------------------------------------------

int main(int argc, char ** argv)
{

    std::map<std::string, int> inverseLookup;

    if(argc != 4){
        std::cerr << std::endl;
        std::cerr << "usage: contigSV your.bam refgenome.fa querygenome.fa"
                  << std::endl;
        return(1);
    }
    /* target is the ref genome */
    /* query is required for inter alignment insertion */

    FastaReference targetFasta ;
    FastaReference queryFasta  ;

    targetFasta.open(argv[2])  ;
    queryFasta.open(argv[3] )  ;


    std::string bname = argv[1];

    BamReader br;

    if(!br.Open(bname)){
        std::cerr << "FATAL: could not open bam file" << std::endl;
        return(1);
    }
    if(!br.LocateIndex()){
        std::cerr << "FATAL: could not locate bam index" << std::endl;
        std::cerr << "INFO:  check that index is bam.bai" << std::endl;
        return(1);
    }

    RefVector rv     = br.GetReferenceData();

    BamAlignment al;

    while(br.GetNextAlignment(al)){
        readInternalPrint(al, targetFasta, rv[al.RefID].RefName);
    }

    return 0;
}
