#include "cigar.hpp"

//------------------------------- SUBROUTINE --------------------------------
/*
 Function input  : The cigar vector and length of the QUERY
 Function does   : Calculates the percent ID
 Function returns: NA
 Function details: [(M + =) - (I+D+X)] / (M + =)
*/

double percentID(std::vector< CigarOp > & cigs, int32_t len){

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

void advanceQuery(const char o,
                  const uint32_t l,
                  long int * pos,
                  bool countHardClip){

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
            *pos += l;
            break;
        }
    case '=':
        {
            *pos += l;
            break;
        }
    case 'X':
        {
            *pos += l;
            break;
        }
    case 'H':
        {
            if(countHardClip){
                *pos += l;
                break;
            }
        }
    default:
        {
            break;
        }
    }
}
//------------------------------- SUBROUTINE --------------------------------
/* default call false*/
void advanceQuery(const char o,
                  const uint32_t l,
                  long int * pos)
{
    advanceQuery(o,l,pos,false);
}

//------------------------------- SUBROUTINE --------------------------------
/*
 Function input  : The cigar vector
 Function does   : Calculates the total length of the query from the cigar
 Function returns: long int
*/

long int qlen(std::vector< CigarOp > & cigs)
{
    long int qLength = 0;
    for(std::vector<CigarOp>::iterator it = cigs.begin();
        it != cigs.end(); it++){
        advanceQuery(it->Type, it->Length, &qLength, true);
    }
    return qLength;
}

//------------------------------- SUBROUTINE --------------------------------
/*
 Function input  : the cigar operation and the length
 Function does   : advances the reference
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
            *pos += l;
            break;
        }
    case 'X':
        {
            *pos += l;
            break;
        }
    case 'D':
        {
            *pos += l;
            break;
        }

    default:
        {
            break;
        }
    }
}

