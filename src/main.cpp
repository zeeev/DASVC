#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include "split.hpp"
#include "cigar.hpp"

#include "api/BamWriter.h"
#include "api/BamMultiReader.h"

using namespace BamTools;

int main(int argc, char ** argv) {

    std::map<std::string, int> inverseLookup;

    if(argc < 2){
        std::cerr << std::endl;
        std::cerr << "usage: contigSV your.bam" << std::endl;
        std::cerr << " INFO: bamfile: " << argv[1] << std::endl;
        std::cerr << std::endl;
        exit(1);
    }

    std::string bname = argv[1];

    BamReader br;

    if(!br.Open(bname)){
        std::cerr << "FATAL: could not open bam file" << std::endl;
        exit(1);
    }
    if(!br.LocateIndex()){
        std::cerr << "FATAL: could not locate bam index" << std::endl;
        std::cerr << "INFO:  check that index is bam.bai" << std::endl;
        exit(1);
    }

    RefVector rv     = br.GetReferenceData();
    SamHeader header = br.GetHeader();

    BamAlignment al;

    while(br.GetNextAlignment(al)){


        std::cerr << al.Name << "\t" << al.Position << "\t" << al.GetEndPosition() << "\t" << al.Length << "\t" << percentID(al.CigarData, al.Length) << std::endl;

        long int qPos = 0;
        long int rPos = al.Position;

        for(std::vector< CigarOp >::iterator it = al.CigarData.begin();
            it != al.CigarData.end(); it++){

            if(it->Type == 'I'){
                std::cout << "INSERTION" << "\t" << rPos << "\t" << al.QueryBases.substr(qPos, it->Length) << std::endl;
            }

            advanceQuery(it->Type, it->Length, &qPos);
            advanceReference(it->Type, it->Length, &rPos);



        }

    }

}
