###############################################
##  YaSat -- Yet Another SAT Solver
##
##  Jack Q (qiaobo@outlook.com)
##  (0540017, CS, NCTU)
###############################################

# Make file

################### Variables #################

CXX=g++
DIRSRC=src
DIRBIN=.
DIROBJ=obj
DIRTEST=test
SHELL=/bin/bash

DIRVERIFIER=verifier
BINVERIFIER=$(DIRVERIFIER)/yasat-veri

EXENAME=yasat

# Debugging flags
FLAGS=  -Wall -Wold-style-cast -Wextra -Wformat=2 \
         -ggdb3 -DDEBUG -std=c++11

# Optimizing flags
# FLAGS=-Wall -Wold-style-cast -Wformat=2 -ansi -pedantic -O3

# List all the .o files you need to build here
HEADER_R:= util.h
HEADERS := yasat.h parser.h clause.h solver.h writer.h\
		   util/exception.h util/io.h
OBJS    := main.o $(HEADERS:.h=.o)
HEADER_R:= $(addprefix $(DIRSRC)/, $(HEADER_R))
HEADERS := $(addprefix $(DIRSRC)/, $(HEADERS)) $(HEADER_R)

.PHONY: clean test dirs tags all verifier verifier-clean

.DEFAULT_GOAL: all

#################### Targets ##################
all: $(DIRBIN)/$(EXENAME) tags

$(DIRBIN)/$(EXENAME): $(addprefix $(DIROBJ)/, $(OBJS))
	$(CXX) $(FLAGS) -o $@ $^

$(DIROBJ)/%.o: $(DIRSRC)/%.cpp $(HEADERS) | dirs
	$(CXX) $(FLAGS) -c -o $@ $<


##################### PHONY ####################
tags:
	@gtags
clean: verifier-clean
	@rm -rf $(addprefix $(DIROBJ)/, $(OBJS))
	@rm -rf $(DIRBIN)/$(EXENAME)
	@rm -rf $(DIROBJ)
	@rm -rf GPATH  GRTAGS  GSYMS  GTAGS
	@rm -rf $(DIRTEST)
test: test-sanity test-tiny
test-sanity: all verifier | dir-test
	@echo "###### SANITY TEST CASE ######"
	@echo "TEST CASE 1"
	$(DIRBIN)/$(EXENAME) benchmarks/sanity/sanity2.cnf $(DIRTEST)/sanity2.sat
	$(BINVERIFIER) -S benchmarks/sanity/sanity2.cnf $(DIRTEST)/sanity2.sat
	@echo "TEST CASE 2"
	$(DIRBIN)/$(EXENAME) benchmarks/sanity/sanity3.cnf $(DIRTEST)/sanity3.sat
	$(BINVERIFIER) -S benchmarks/sanity/sanity3.cnf $(DIRTEST)/sanity3.sat
	@echo "TEST CASE 3"
	$(DIRBIN)/$(EXENAME) benchmarks/sanity/sanity4.cnf $(DIRTEST)/sanity4.sat
	$(BINVERIFIER) -U benchmarks/sanity/sanity4.cnf $(DIRTEST)/sanity4.sat
	@echo "TEST CASE 4"
	$(DIRBIN)/$(EXENAME) benchmarks/sanity/sanity5.cnf $(DIRTEST)/sanity5.sat
	$(BINVERIFIER) -U benchmarks/sanity/sanity5.cnf $(DIRTEST)/sanity5.sat
test-tiny: all verifier | dir-test
	@echo "####### TINY TEST CASE #######"
	@echo "TEST CASE 1"
	$(DIRBIN)/$(EXENAME) benchmarks/tiny/rand5_10.cnf $(DIRTEST)/rand5_10.sat
	$(BINVERIFIER) -S benchmarks/tiny/rand5_10.cnf $(DIRTEST)/rand5_10.sat
	@echo "TEST CASE 2"
	$(DIRBIN)/$(EXENAME) benchmarks/tiny/rand5_30.cnf $(DIRTEST)/rand5_30.sat
	$(BINVERIFIER) -U benchmarks/tiny/rand5_30.cnf $(DIRTEST)/rand5_30.sat
	@echo "TEST CASE 3"
	$(DIRBIN)/$(EXENAME) benchmarks/tiny/rand10_20.cnf $(DIRTEST)/rand10_20.sat
	$(BINVERIFIER) -S benchmarks/tiny/rand10_20.cnf $(DIRTEST)/rand10_20.sat
	@echo "TEST CASE 4"
	$(DIRBIN)/$(EXENAME) benchmarks/tiny/rand10_50.cnf $(DIRTEST)/rand10_50.sat
	$(BINVERIFIER) -U benchmarks/tiny/rand10_50.cnf $(DIRTEST)/rand10_50.sat
test-crafted: all verifier | dir-test
	@echo "###### CRAFTED TEST CASE ######"
	@echo "TEST CASE 1: fixed bandwidth"
	$(DIRBIN)/$(EXENAME) benchmarks/crafted/fixed-bandwidth.cnf $(DIRTEST)/fixed-bandwidth.sat
	$(BINVERIFIER) -S benchmarks/crafted/fixed-bandwidth.cnf $(DIRTEST)/fixed-bandwidth.sat
	@echo "TEST CASE 2: ecgrid"
	$(DIRBIN)/$(EXENAME) benchmarks/crafted/ecgrid.cnf $(DIRTEST)/ecgrid.sat
	$(BINVERIFIER) -S benchmarks/crafted/ecgrid.cnf $(DIRTEST)/ecgrid.sat
	@echo "TEST CASE 3: tseiting grid"
	$(DIRBIN)/$(EXENAME) benchmarks/crafted/tseiting-grid.cnf $(DIRTEST)/tseiting-grid.sat
	$(BINVERIFIER) -S benchmarks/crafted/tseiting-grid.cnf $(DIRTEST)/tseiting-grid.sat
	@echo "TEST CASE 4: rphp4"
	$(DIRBIN)/$(EXENAME) benchmarks/crafted/rphp4.cnf $(DIRTEST)/rphp4.sat
	$(BINVERIFIER) -S benchmarks/crafted/rphp4.cnf $(DIRTEST)/rphp4.sat
	@echo "TEST CASE 5: ptn"
	$(DIRBIN)/$(EXENAME) benchmarks/crafted/ptn.cnf $(DIRTEST)/ptn.sat
	$(BINVERIFIER) -S benchmarks/crafted/ptn.cnf $(DIRTEST)/ptn.sat

	@cat $(DIRTEST)/tiny1.sat
dirs:
	@mkdir -p $(DIROBJ)/{,util}
dir-test:
	@mkdir -p $(DIRTEST)

################### verifier ###################
verifier:
	@make --no-print-directory -C $(DIRVERIFIER)
verifier-clean:
	@make --no-print-directory -C $(DIRVERIFIER) clean
