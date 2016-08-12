#include "errors.hpp"

errorHandler::errorHandler(void){

    usage_statement = "usage: contigSV annotate";

    error_message.push_back( std::string("FATAL: Could not open bam file.")              );
    error_message.push_back( std::string("FATAL: Could not open bam index.")             );
    error_message.push_back( std::string("FATAL: Could not read bam file - or no reads."));
    error_message.push_back( std::string("FATAL: Bad command line option."              ));
    error_message.push_back(usage_statement                                              );
}

void errorHandler::croak(errors i, bool ex){
    std::cerr << error_message[i] << std::endl;
    if(ex){
        exit(1);
    }
}

void errorHandler::croak(errors i, bool ex, std::string m){
    std::cerr << error_message[i] << " : " << m << std::endl;
    if(ex){
        exit(1);
    }
}

void errorHandler::printInfo(std::string m){
    std::cerr << "INFO: " << m << std::endl;
}
