#include "cigar.hpp"

//------------------------------- SUBROUTINE --------------------------------
/*
 Function input  : The cigar vector and length of the QUERY
 Function does   : Calculates the percent ID
 Function returns: NA
 Function details: [(M + =) - (I+D+X)] / (M + =)
*/

double percentID(std::vector< CigarOp > & cigs, int32_t len){

    double percentID = 0;
    double num       = 0;
    double denom     = 0;

    for(std::vector<CigarOp>::iterator it = cigs.begin();
        it != cigs.end(); it++){
        switch(it->Type){
        case 'M':
            {
                num += it->Length;
                break;
            }
        case '=':
            {
                num += it->Length;
                break;
            }
        case 'X':
            {
                denom += it->Length;
                break;
            }
        case 'I':
            {
                denom += it->Length;
                break;
            }
        case 'D':
            {
                denom += it->Length;
                break;
            }

        default:
            {
                break;
            }
        }
    }




    return num / (denom + num);
}

//------------------------------- SUBROUTINE --------------------------------
/*
 Function input  : the cigar operation and the length
 Function does   : advances the query position
 Function returns: NA
*/

void advanceQuery(const char o, const uint32_t l, long int * pos){

    switch(o){
    case 'M':
        {
            *pos += l;
            break;
        }
    case 'I':
        {
            *pos += l;
            break;
        }
    case 'S':
        {
            *pos += 1;
            break;
        }
    case '=':
        {
            *pos += 1;
            break;
        }
    case 'X':
        {
            *pos += 1;
            break;
        }
    default:
        {
            break;
        }
    }
}

/*
 Function input  : the cigar op and the length
 Function does   : advances the reference position
 Function returns: NA
*/


void advanceReference(const char o, const uint32_t l, long int * pos){

    switch(o){
    case 'M':
        {
            *pos += l;
            break;
        }
    case '=':
        {
            *pos += 1;
            break;
        }
    case 'X':
        {
            *pos += 1;
            break;
        }
    case 'D':
        {
            *pos += 1;
            break;
        }

    default:
        {
            break;
        }
    }
}

