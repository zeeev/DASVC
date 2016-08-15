#ifndef ERRORS_H
#define ERRORS_H

#include <string>
#include <vector>
#include <iostream>

class errorHandler{
public:
    enum errors{
        COULD_NOT_OPEN_BAM,
        COULD_NOT_OPEN_BAM_INDEX,
        NO_READS,
        BAD_COMMAND_LINE_OPTION,
        BAM_NOT_ANNOTATED,
        USAGE
    };

    enum info{
        RUNNING_VARCALL,
        RUNNING_ANNOTATE,
    };

    std::vector<std::string> error_message ;
    std::vector<std::string> info_message  ;

    std::string usage_statement ;

    errorHandler(void);

    void croak(errors, bool              );
    void croak(errors, bool, std::string );
    void printInfo(          std::string );

};

#endif
