#include <map>
#include <string>
#include "errors.hpp"
#include "annotate.hpp"
#include "varcall.hpp"

std::map< std::string, int > PROGS;

int main(int argc, char ** argv)
{
    errorHandler EH;

    std::map< std::string, int > PROGS;

    PROGS[ "annotate" ] = 1;
    PROGS[ "varcall"  ] = 2;

    if(argc < 2){
        EH.croak(EH.USAGE, true);
    }

    std::string prog = argv[1];

    if(PROGS.find(prog) == PROGS.end()){
        EH.croak(EH.BAD_COMMAND_LINE_OPTION, true, prog);
    }

    switch(PROGS[argv[1]]){
    case 1:
        {
            return annotate((std::string)argv[2],
                            (std::string)argv[3],
                            (std::string)argv[4]);
            break;
        }
    case 2:
        {
            std::string bf = argv[2];
            std::string tf = argv[3];
            std::string qf = argv[4];

            return varcall(bf, tf, qf);
            break;
        }

    default:
        {
            /* should never hit here */
        }
    }

    return 0;
}
