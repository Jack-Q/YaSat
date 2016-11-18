#include "parser.h"
#include "solver.h"
#include "writer.h"
#include "yasat.h"

namespace yasat {

inline void printTitle(ostream &out);
inline void printHelp(ostream &out);
inline void printClauses(ostream &out, vector<Clause> &clauses);

YaSat::YaSat()
    : state(0), inputDataSource(nullptr), outputResult(nullptr),
      outputMessage(nullptr) {}

int YaSat::parseArgument(char **arguments, int argumentCount) {
  int currentArgument = 1; // ignore application name
  string input, output;
  while (currentArgument < argumentCount) {
    string str = arguments[currentArgument];
    if (str == "--help" || str == "--usage")
      throw RoutineException(RoutineException::USAGE);
    else if (str == "--stdin") {
      this->inputDataSource = &cin;
    } else if (str == "--stdout")
      this->outputResult = &cout;
    else if (str[0] == '-') {
      string message;
      message = message + "Uncongnized commandline option \"" + str + "\"";
      throw OptionException(message);
    } else {
      // Other parameter, treated as file names
      if (input.empty())
        input = str;
      else if (output.empty())
        output = str;
      else {
        string message;
        message = message +
                  "Too much file name specified, first extra file name is \"" +
                  str + "\"";
        throw OptionException(message);
      }
    }

    currentArgument++;
  }

  // Determine input source
  if (inputDataSource) {
    if (!output.empty())
      throw OptionException(
          "Cannot specific both input file name and `--stdin' option");
    output = input;
  } else {
    if (!input.empty()) {
      std::ifstream *inputFile = new std::ifstream(input);
      if (!inputFile->is_open()) {
        string message;
        message = message + "Cannot open input file \"" + input + "\'";
        throw OptionException(message);
      }
      inputDataSource = inputFile;
    } else {
      throw OptionException("Please specific input data");
    }
  }

  // Determine input source
  if (outputResult) {
    if (!output.empty())
      throw OptionException(
          "Cannot specific both output file name and `--stdout' option");
    outputMessage = new std::ofstream("message.log", std::ios::app);
  } else {
    if (!output.empty()) {
      std::ofstream *outputFile = new std::ofstream(output);
      if (!outputFile->is_open()) {
        string message;
        message = message + "Cannot open input file \"" + output + "\'";
        throw OptionException(message);
      }
      outputResult = outputFile;
      outputMessage = &cout;
    } else {
      throw OptionException("Please specific output file name");
    }
  }

  return 0;
}

void YaSat::printError() const {
  printTitle(std::cout);
  cout << "Error: *TODO* " << endl;
}

void YaSat::printUsage() const {
  printTitle(cout);
  printHelp(cout);
}

void YaSat::loadClause() {
  Parser parser(message());
  parser.parse(*inputDataSource, clauses);
  message() << fmt::messageLabel << "Clauses after load:" << endl;
  printClauses(message(), clauses);
  maxLiterial = parser.getMaxLiterial();
}

void YaSat::solve() {
  Solver solver(clauses, maxLiterial, message());

  solver.prep();
  message() << fmt::messageLabel << "Clauses after preparation step:" << endl;
  printClauses(message(), clauses);
  solver.solve();
  message() << fmt::messageLabel << "Clauses after soling step:" << endl;
  printClauses(message(), clauses);
  solver.getSolution(solution);
}

void YaSat::printResult() {
  Writer writer(message());
  message() << fmt::messageLabel << "Write result to output" << endl;
  writer.write(result(), solution);
}

// private small functions

inline void printTitle(ostream &out) {
  out << fmt::message
      << "########################################################" << endl
      << "##                        YaSat                       ##" << endl
      << "########################################################" << endl
      << fmt::reset
      << "                              Jack Q (0540017, CS, NCTU)" << endl
      << endl;
}

inline void printHelp(ostream &out) {
  out << "Usage:                                                  " << endl
      << "  yasat INPUT OUTPUT                                    " << endl
      << "    >> read from data file and write to file            " << endl
      << "  yasat --stdin --stdout                                " << endl
      << "    >> read from `stdin' and write to `stdout'          " << endl
      << "  yasat --help                                          " << endl
      << "    >> print usage information                          " << endl
      << endl;
}

inline void printClauses(ostream &out, vector<Clause> &clauses) {
  for (auto cls = clauses.begin(); cls != clauses.end(); cls++)
    out << *cls << endl;
}
}
