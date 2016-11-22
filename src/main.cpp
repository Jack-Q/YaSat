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

int main(int argc, char *argv[]) {
  yasat::YaSat solver;
  try {
    solver.parseArgument(argv, argc);
    solver.loadClause();
    solver.solve();
    solver.printResult();
    return 0;
  } catch (const yasat::RoutineException &ex) {
    switch (ex.getType()) {
    case yasat::RoutineException::USAGE:
      solver.printUsage();
      break;
    case yasat::RoutineException::CANCELLATION:
      yasat::cout << yasat::fmt::messageLabel << "USER CANCELLATION"
                  << yasat::endl;
      break;
    }
    return 0;
  } catch (const yasat::OptionException &ex) {
    yasat::cerr << yasat::fmt::errorLabel
                << "Command Option Exception: " << yasat::endl
                << ex.what() << yasat::endl;
    solver.printUsage();
  } catch (const yasat::Exception &ex) {
    yasat::cerr << yasat::fmt::errorLabel
                << "Internal Exception: " << yasat::endl
                << ex.what() << yasat::endl;
    if (ex.showUsage())
      solver.printUsage();
  }
  #if !defined(DEBUG)
  // disabled in debug mode to pass exception to debugger
  catch (const std::exception &ex) {
    yasat::cerr << yasat::fmt::errorLabel
                << "Internal Exception: " << yasat::endl
                << ex.what() << yasat::endl;
  }
  catch (...) {
    yasat::cerr << yasat::fmt::errorLabel << "Unknow unhandled exception"
                << yasat::endl;
    throw;
  }
  #endif
  return 1;
}
