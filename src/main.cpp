/**********************************************
 *  YaSat -- Yet Another SAT Solver
 *
 *  Jack Q (qiaobo@outlook.com)
 *  (0540017, CS, NCTU)
 **********************************************
 * main.cpp
 * solver program entry point
 **********************************************/

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "yasat.h"
#include "util/exception.h"

int main(int argc, char *argv[])
{
  yasat::YaSat solver;
  try
  {
    solver.parseArgument(argv, argc);
    solver.loadClause();
    solver.solve();
    solver.printResult();
    return 0;
  }
  catch (const yasat::RoutineException &ex)
  {
    switch (ex.getType())
    {
    case yasat::RoutineException::USAGE:
      solver.printUsage();
      break;
    case yasat::RoutineException::CANCELLATION:
      std::cout << "USER CANCELLATION" << std::endl;
      break;
    }
    return 0;
  }
  catch (const yasat::Exception &ex)
  {
    std::cerr << "Internal Exception: " << std::endl
              << ex.what() << std::endl;
  }
  catch (const std::exception &ex)
  {
    std::cerr << "Internal Exception: " << std::endl
              << ex.what() << std::endl;
  }
  catch (...)
  {
    std::cerr << "Unknow unhandled exception" << std::endl;
    throw;
  }
  return 1;
}
