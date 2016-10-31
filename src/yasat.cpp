#include <iostream>
#include "yasat.h"
#include "util/exception.h"

namespace yasat
{

inline void printTitle(std::ostream &out);

YaSat::YaSat() : state(0), clauseSource(nullptr)
{
}

int YaSat::parseArgument(char **arguments, int argumentCount)
{

    if (argumentCount > 0)
        std::cout << arguments[0] << std::endl;
    throw RoutineException(RoutineException::USAGE);
    return 0;
}

void YaSat::printError() const
{
    printTitle(std::cout);
    std::cout << "Error: *TODO* " << std::endl;
}

void YaSat::printUsage() const
{
    printTitle(std::cout);
    std::cout << "Usage: *TODO* " << std::endl;
}

void YaSat::loadClause()
{
    if(clauseSource == nullptr){
        
    }
}

void YaSat::solve()
{

}

void YaSat::printResult()
{

}

// private small functions

inline void printTitle(std::ostream &out)
{
    out << "#######################################" << std::endl
        << "##              YaSat                ##" << std::endl
        << "#######################################" << std::endl
        << "             Jack Q (0540017, CS, NCTU)" << std::endl;
}
}