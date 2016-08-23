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
 Function input  : vector of BamAlignments
 Function does   : chains the query
 Function returns: error code
 Function details: runs chaining over the alignments then saves them;

*/

int chainBlock(std::vector<BamAlignment> & reads,
               BamWriter & br,
                const RefVector & references){

    if(reads.size() == 0){
        return 1;
    }

    chain qChain;

    for(std::vector<BamAlignment>::iterator it = reads.begin();
        it != reads.end(); it++){

        int match ;
        int qStart;
        int qEnd  ;

        it->GetTag<int>("MB", match  );
        it->GetTag<int>("QS", qStart );
        it->GetTag<int>("QE", qEnd   );

        qChain.addAlignment(qStart, qEnd, match);

    }

    std::vector<int> indiciesOfAlignments;
    qChain.buildLinks();
    qChain.traceback(indiciesOfAlignments);

    std::cerr << " INFO: n alignments before chaining :"
              << reads.size() << " and after : "
              << indiciesOfAlignments.size()
              << " for " << reads.front().Name << std::endl;


    int totalMatchingBases = 0;

    for(std::vector<int>::iterator it = indiciesOfAlignments.begin();
        it != indiciesOfAlignments.end(); it++){
        int mb = 0;
        reads[*it].GetTag<int>("MB", mb);
        totalMatchingBases += mb;
    }

    sort(indiciesOfAlignments.begin(), indiciesOfAlignments.end());

    int index = 1;

    for(std::vector<int>::iterator it = indiciesOfAlignments.begin();
        it != indiciesOfAlignments.end(); it++){
        reads[*it].AddTag<int>("TM", "i", totalMatchingBases);
        reads[*it].AddTag<int>("AI", "i", index);
        index++;
        br.SaveAlignment(reads[*it]);
    }

    int qs;
    int qe;

    reads.front().GetTag<int>("QS", qs);
    reads.back().GetTag<int>("QE",  qe);

    std::cerr << " ANNOTATION: T_S_E : Q_S_E : "
              << references[reads.front().RefID].RefName
              << "_"
              << reads.front().Position
              << "_"
              << reads.back().GetEndPosition()
              << " : "
              << reads.front().Name
              << "_"
              << qs
              << "_"
              << qe << std::endl;

    return 0;
}



//------------------------------- SUBROUTINE --------------------------------
/*
 Function input  : list of BamAlignments
 Function does   : sorts and orients blocks by query position
 Function returns: error code
 Function details: Sorts alignments by first match position then adds
                   the new optional tag.

*/

int processBlock(std::list< BamAlignment > & readBuffer ,
                 std::vector< BamAlignment > & processed,
                 int blockId)
{

    int totalAlignedBases = 0;

    std::vector<BamAlignment> reads;

    std::map<int, int> matchesPerContig;

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

        if(matchesPerContig.find(it->RefID) != matchesPerContig.end() ){
            matchesPerContig[it->RefID] += matchingBases;
        }
        else{
            matchesPerContig[it->RefID] = matchingBases;
        }

        /* position where the alignment starts matching */

        it->AddTag<int>( "QS", "i", queryStart      );

        /* position in the aligment where it stops matching */

        it->AddTag<int>( "QE", "i", queryEnd        );

        /* lenght of the alignment block */

        it->AddTag<int>( "QL", "i", (queryEnd - queryStart));


        it->AddTag<int>( "TS", "i", it->Position);
        it->AddTag<int>( "TE", "i", it->GetEndPosition());

        /* number of matching bases M= */

        it->AddTag<int>( "MB", "i", matchingBases);

        /* percent ID */

        it->AddTag<float>( "PI", "f", pctID);

        /* alignment group ; just another way of saying query name */

        it->AddTag<int>( "BI", "i", blockId);

        if(matchingBases > 100 && pctID > 0.90 ){
            totalAlignedBases += matchingBases;
            reads.push_back(*it);
        }
    }

    sort(reads.begin(), reads.end(),  qStartSort);

    /* best contig match */

    int maxRefId = 0;
    int maxValue = 0;

    for(std::map<int, int>::iterator mp = matchesPerContig.begin();
        mp != matchesPerContig.end(); mp++){
        if(mp->second > maxValue){
            maxRefId = mp->first;
            maxValue = mp->second;
        }
    }

    for(int i = 0; i < reads.size(); i++){

        if(reads[i].RefID != maxRefId){
            continue;
        }
        processed.push_back(reads[i]);
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

int annotate(std::string bfName, std::string out)
{
    BamReader reader;

    errorHandler EH;

    if(!reader.Open(bfName)){
        EH.croak(EH.COULD_NOT_OPEN_BAM, true);
    }

    const SamHeader header     = reader.GetHeader();
    const RefVector references = reader.GetReferenceData();

    BamWriter writer;
    if ( !writer.Open(out, header, references) ) {
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

                std::vector<BamAlignment> filtReads;

                processBlock(readBuffer, filtReads, blockId);
                chainBlock(filtReads, writer,  references);

                readBuffer.clear()      ;
                readBuffer.push_back(al);
            }
            else{
                 readBuffer.push_back(al);
            }
        }
    }

    blockId += 1;

    std::vector<BamAlignment> filtReadsB;
    processBlock(readBuffer, filtReadsB, blockId );
    chainBlock(filtReadsB, writer, references    );

    reader.Close();
    writer.Close();

    return 0;
}
