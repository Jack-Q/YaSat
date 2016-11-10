/**********************************************
 *  YaSat -- Yet Another SAT Solver
 *
 *  Jack Q (qiaobo@outlook.com)
 *  (0540017, CS, NCTU)
 **********************************************
 * main.cpp
 * solver program entry point
 **********************************************/

#include <stdexcept>

#include "yasat.h"

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
      yasat::cout << "USER CANCELLATION" << yasat::endl;
      break;
    }
    return 0;
  }
  catch (const yasat::Exception &ex)
  {
    yasat::cerr << "Internal Exception: " << yasat::endl
              << ex.what() << yasat::endl;
    if(ex.showUsage())
      solver.printUsage();
  }
  catch (const std::exception &ex)
  {
    yasat::cerr << "Internal Exception: " << yasat::endl
              << ex.what() << yasat::endl;
  }
  catch (...)
  {
    yasat::cerr << "Unknow unhandled exception" << yasat::endl;
    throw;
  }
  return 1;
}
