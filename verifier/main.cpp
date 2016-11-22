/**********************************************
 *  YaSat -- Yet Another SAT Solver
 *
 *  Jack Q (qiaobo@outlook.com)
 *  (0540017, CS, NCTU)
 **********************************************
 * main.cpp
 * SatSolver output verifier
 **********************************************/

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cerrno>
#include <cstring>

#ifdef DEBUG
#define DEBUG_INFO(x) std::cout<<FMT(gre, [MESSAGE])<<" " x << std::endl
#else
#define DEBUG_INFO(x)
#endif

extern int errno;

// Reset
#define FMT_rst "\e[0m"

// Normal
#define FMT_blk "\e[0;30m"
#define FMT_blu "\e[0;34m"
#define FMT_cya "\e[0;36m"
#define FMT_gre "\e[0;32m"
#define FMT_pur "\e[0;35m"
#define FMT_red "\e[0;31m"
#define FMT_whi "\e[0;37m"
#define FMT_yel "\e[0;33m"

// Shade
#define FMT_sl_blk "\e[40m"
#define FMT_sl_blu "\e[44m"
#define FMT_sl_cya "\e[46m"
#define FMT_sl_gre "\e[42m"
#define FMT_sl_pur "\e[45m"
#define FMT_sl_red "\e[41m"
#define FMT_sl_whi "\e[47m"
#define FMT_sl_yel "\e[43m"
#define FMT_sl_def "\e[49m"

#define __FMT_cat(a, ...) a##__VA_ARGS__
#define __FMT(fg, bg, str)                                                     \
  FMT_rst __FMT_cat(FMT_, fg) __FMT_cat(FMT_sl_, bg) #str FMT_rst "\e[40m"
#define __FMT_sel(_1, _2, _3, name, ...) name
#define __FMT_1(c) __FMT_cat(FMT_, c)
#define __FMT_2(fg, str) __FMT(fg, def, str)
#define __FMT_3(fg, bg, str) __FMT(fg, bg, str)
#define FMT(...) __FMT_sel(__VA_ARGS__, __FMT_3, __FMT_2, __FMT_1)(__VA_ARGS__)

void printUsage(const char *progname = "yasat-veri") {
  std::cout << FMT(gre, [USAGE]) << " " << progname
            << " [ -S | -N ] [CNF INPUT] [SAT OUTPUT]" << std::endl;
  std::cout << "  >> " << FMT(whi, [-S | -N])
            << "  : the expected result of the input cnf file, `-S' for "
            << "Satisfiabile and `-U' for unsatisfiable." << std::endl;
  std::cout << "  >> " << FMT(whi, [CNF INPUT])
            << " : SAT problem data in CNF format" << std::endl;
  std::cout << "  >> " << FMT(whi, [SAT OUTPUT])
            << ": result file output by the SAT solver" << std::endl;
  std::cout << "     " << FMT(whi, EXAMPLE) << ": " << progname
            << " -S input.cnf result.sat " << std::endl;
}

int result(bool isMatch){
  if(isMatch){
    DEBUG_INFO("the result is verified! ");
    return 0;
  }else{
    std::cout << FMT(red, [result]) << " mismatch" << std::endl;
    return 2;
  }
}

int main(int argc, const char **argv) {
  if (argc != 4) {
    std::cout << FMT(red, [ERROR])
              << " unsupport usage with arguments other then 3" << std::endl;
    printUsage(argv[0]);
    return 1;
  }


  std::string isSatOption = argv[1];
  std::string cnfFileName = argv[2];
  std::string satFileName = argv[3];


  bool isSat = false;
  if(isSatOption == "-U" ){
    DEBUG_INFO("expecting unsat");
    isSat = false;
  }else if(isSatOption == "-S"){
    DEBUG_INFO("expecting sat");
    isSat = true;
  }else{
    std::cout << FMT(red, [ERROR]) << " unexpected option " << isSatOption << std::endl;
    printUsage();
    return 1;
  }

  std::ifstream satFile(satFileName);
  if(!satFile.is_open()){
    std::cout << FMT(red, [ERROR]) << " failed to open input sat file: " << strerror(errno) << std::endl;
    return 1;
  }

  std::string statusLine;
  std::getline(satFile >> std::ws, statusLine);
  std::transform(statusLine.begin(), statusLine.end(), statusLine.begin(), ::tolower);
  // Skip the leading label (for status line, the label is s)
  if(statusLine.find("s ") == 0){
    statusLine = statusLine.substr(2, statusLine.size() - 2);
  }
  if(statusLine.find("sat") == 0){
    DEBUG_INFO("result says SAT");
    if(isSat == false){
      return result(false);
    }
  }else if(statusLine.find("unsat") == 0){
    DEBUG_INFO("result says UNSAT");
    if(isSat == true){
      return result(false);
    }else{
      satFile.close();
      return result(true);
    }
  }else{
    std::cout << FMT(red, [ERROR]) << " cannot parse result file header" << std::endl;
    return 1;
  }

  std::vector<bool> table;
  satFile >> std::ws;
  // Skip the leading label (for assignment line, this label is v)
  if(!isdigit(satFile.peek())){
    char label;
    satFile >> label;
  }
  while(!satFile.eof()){
    int assign = 0;
    satFile >> assign;
    if(assign == 0){
      break;
    }
    if(static_cast<size_t>(abs(assign)) >= table.size())
      table.resize(abs(assign) + 1);
    table[abs(assign) - 1] = assign > 0;
  }
  satFile.close();

  std::ifstream cnfFile(cnfFileName);
  if(!cnfFile.is_open()){
    std::cout << FMT(red, [ERROR]) << " failed to open input cnf file: " << strerror(errno) << std::endl;
    return 1;
  }

  std::string cnfLine;
  char cnfLabel;
  while (true) {
    cnfFile >> std::ws;
    if (cnfFile.eof())
      break;
    cnfLabel = cnfFile.peek();
    switch (cnfLabel) {
    case 'c':
    case 'p':
      cnfFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    default:
      std::getline(cnfFile >> std::ws, cnfLine);
      {
        std::istringstream str(cnfLine);
        bool isLineSat = false;
        int number = -1;
        while (true) {
          str >> number;
          if (number == 0)
            break;
          if(static_cast<size_t>(abs(number)) > table.size()){
            std::cout << FMT(red, [ERROR]) << " result hasn't cover all of the literials" << std::endl;
            return result(false);
          }
          if(number > 0 && table[number - 1]){
            isLineSat = true;
            break;
          }else if(number < 0 && !table[-number - 1]){
            isLineSat = true;
            break;
          }
          str >> std::ws;
          if (str.eof())
            break;
        }
        if(!isLineSat){
            std::cout << FMT(red, [ERROR]) << " the assignement in result cannot satisfy clause : " << FMT(yel) << cnfLine << FMT(rst) << std::endl;
        return result(false);
        }
      }
    }
  }

  return result(true);
}
