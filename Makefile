###############################################
##  YaSat -- Yet Another SAT Solver
##
##  Jack Q (qiaobo@outlook.com)
##  (0540017, CS, NCTU)
###############################################

# Make file

################### Variables #################

# General
CXX=g++
DIRSRC=src
DIRBIN=.
DIROBJ=obj
DIRTEST=test
SHELL=/bin/bash
EXENAME=yasat

# Verifier
DIRVERIFIER=verifier
BINVERIFIER=$(DIRVERIFIER)/yasat-veri

# Compiling Option
# set to any non-blank value to toggle these option
FLAGS_DEBUG=
FLAGS_COLOR=
FLAGS_VERBOSE=
FLAGS_PARSE_EXT_COMPAT=1
FLAGS_PRINT_STATIS=
FLAGS_LITERAL_WEIGHT_DECAY=1
FLAGS_LITERAL_WEIGHT_UPDATE=1

# Common basic flags
FLAGS:=  -Wall -Wold-style-cast -Wextra -Wformat=2 \
          -std=c++14
ifneq ("$(FLAGS_DEBUG)","")
	FLAGS := $(FLAGS) -DDEBUG -ggdb3 # Debug flags
else
	FLAGS := $(FLAGS) -O2 -O3 -Ofast -flto # Release flags
endif
ifneq ("$(FLAGS_COLOR)","")
	FLAGS := $(FLAGS) -DDEBUG_COLOR
endif
ifneq ("$(FLAGS_VERBOSE)","")
	FLAGS := $(FLAGS) -DDEBUG_VERBOSE
endif
ifneq ("$(FLAGS_PARSE_EXT_COMPAT)", "")
	FLAGS := $(FLAGS) -DPARSE_EXT_COMPAT
endif
ifneq ("$(FLAGS_PRINT_STATIS)", "")
	FLAGS := $(FLAGS) -DPRINT_STATIS
endif
ifneq ("$(FLAGS_LITERAL_WEIGHT_DECAY)", "")
	FLAGS := $(FLAGS) -DLITERAL_WEIGHT_DECAY
endif
ifneq ("$(FLAGS_LITERAL_WEIGHT_UPDATE)", "")
	FLAGS := $(FLAGS) -DLITERAL_WEIGHT_UPDATE
endif

# List all the .o files you need to build here
HEADER_R:= util.h
HEADERS := yasat.h parser.h clause.h solver.h writer.h\
		   util/exception.h util/io.h
OBJS    := main.o $(HEADERS:.h=.o)
HEADER_R:= $(addprefix $(DIRSRC)/, $(HEADER_R))
HEADERS := $(addprefix $(DIRSRC)/, $(HEADERS)) $(HEADER_R)

.PHONY: clean dirs tags all verifier verifier-clean

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

.PHONY: test test-m2 test-sanity test-tiny test-crafted test-m2-simple test-m2-hard
test: test-sanity test-tiny test-m2-simple
test-m2: test-m2-simple test-m2-hard
test-sanity: all verifier | dir-test
	@echo "###### SANITY TEST CASE ######"
	@echo "TEST CASE 1"
	$(DIRBIN)/$(EXENAME) benchmarks/sanity/sanity2.cnf $(DIRTEST)/sanity2.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/sanity/sanity2.cnf $(DIRTEST)/sanity2.sat
	@echo "TEST CASE 2"
	$(DIRBIN)/$(EXENAME) benchmarks/sanity/sanity3.cnf $(DIRTEST)/sanity3.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/sanity/sanity3.cnf $(DIRTEST)/sanity3.sat
	@echo "TEST CASE 3"
	$(DIRBIN)/$(EXENAME) benchmarks/sanity/sanity4.cnf $(DIRTEST)/sanity4.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/sanity/sanity4.cnf $(DIRTEST)/sanity4.sat
	@echo "TEST CASE 4"
	$(DIRBIN)/$(EXENAME) benchmarks/sanity/sanity5.cnf $(DIRTEST)/sanity5.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/sanity/sanity5.cnf $(DIRTEST)/sanity5.sat
test-tiny: all verifier | dir-test
	@echo "####### TINY TEST CASE #######"
	@echo "TEST CASE 1"
	$(DIRBIN)/$(EXENAME) benchmarks/tiny/rand5_10.cnf $(DIRTEST)/rand5_10.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/tiny/rand5_10.cnf $(DIRTEST)/rand5_10.sat
	@echo "TEST CASE 2"
	$(DIRBIN)/$(EXENAME) benchmarks/tiny/rand5_30.cnf $(DIRTEST)/rand5_30.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/tiny/rand5_30.cnf $(DIRTEST)/rand5_30.sat
	@echo "TEST CASE 3"
	$(DIRBIN)/$(EXENAME) benchmarks/tiny/rand10_20.cnf $(DIRTEST)/rand10_20.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/tiny/rand10_20.cnf $(DIRTEST)/rand10_20.sat
	@echo "TEST CASE 4"
	$(DIRBIN)/$(EXENAME) benchmarks/tiny/rand10_50.cnf $(DIRTEST)/rand10_50.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/tiny/rand10_50.cnf $(DIRTEST)/rand10_50.sat
test-m2-simple: all verifier | dir-test
	@echo "###### M2-SIMPLE TEST CASE ######"
	@echo "TEST CASE 1: John Hooker 1"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/jnh1.cnf $(DIRTEST)/jnh1.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/jnh1.cnf $(DIRTEST)/jnh1.sat
	@echo "TEST CASE 2: John Hooker 10"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/jnh10.cnf $(DIRTEST)/jnh10.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/m2-simple/jnh10.cnf $(DIRTEST)/jnh10.sat
	@echo "TEST CASE 3: John Hooker 11"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/jnh11.cnf $(DIRTEST)/jnh11.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/m2-simple/jnh11.cnf $(DIRTEST)/jnh11.sat
	@echo "TEST CASE 4: Inductive Inference (8)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/ii8a1.cnf $(DIRTEST)/ii8a1.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/ii8a1.cnf $(DIRTEST)/ii8a1.sat
	@echo "TEST CASE 5: Inductive Inference (16)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/ii16a1.cnf $(DIRTEST)/ii16a1.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/ii16a1.cnf $(DIRTEST)/ii16a1.sat
	@echo "TEST CASE 6: Inductive Inference (32)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/ii32a1.cnf $(DIRTEST)/ii32a1.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/ii32a1.cnf $(DIRTEST)/ii32a1.sat
	@echo "TEST CASE 7: Dubois (20)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/dubois20.cnf $(DIRTEST)/dubois20.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/m2-simple/dubois20.cnf $(DIRTEST)/dubois20.sat
	@echo "TEST CASE 8: Dubois (100)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/dubois100.cnf $(DIRTEST)/dubois100.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/m2-simple/dubois100.cnf $(DIRTEST)/dubois100.sat
	@echo "TEST CASE 9: Artificial Instance 50 (Satisfiabile)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/aim-50-1_6-yes1-1.cnf $(DIRTEST)/aim-50-1_6-yes1-1.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/aim-50-1_6-yes1-1.cnf $(DIRTEST)/aim-50-1_6-yes1-1.sat
	@echo "TEST CASE 10: Artificial Instance 50 (Unsatisfiabile)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/aim-50-1_6-no-1.cnf $(DIRTEST)/aim-50-1_6-no-1.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/m2-simple/aim-50-1_6-no-1.cnf $(DIRTEST)/aim-50-1_6-no-1.sat
	@echo "TEST CASE 11: Artificial Instance 100 (Satisfiabile)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/aim-100-1_6-yes1-1.cnf $(DIRTEST)/aim-100-1_6-yes1-1.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/aim-100-1_6-yes1-1.cnf $(DIRTEST)/aim-100-1_6-yes1-1.sat
	@echo "TEST CASE 12: Artificial Instance 100 (Unsatisfiabile)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/aim-100-1_6-no-1.cnf $(DIRTEST)/aim-100-1_6-no-1.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/m2-simple/aim-100-1_6-no-1.cnf $(DIRTEST)/aim-100-1_6-no-1.sat
	@echo "TEST CASE 13: Artificial Instance 200 (Satisfiabile)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/aim-200-1_6-yes1-1.cnf $(DIRTEST)/aim-200-1_6-yes1-1.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/aim-200-1_6-yes1-1.cnf $(DIRTEST)/aim-200-1_6-yes1-1.sat
	@echo "TEST CASE 14: Artificial Instance 200 (Unsatisfiabile)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/aim-200-1_6-no-1.cnf $(DIRTEST)/aim-200-1_6-no-1.sat > /dev/null
	$(BINVERIFIER) -U benchmarks/m2-simple/aim-200-1_6-no-1.cnf $(DIRTEST)/aim-200-1_6-no-1.sat
	@echo "TEST CASE 15: Parity 8bit (Simplified)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/par8-1-c.cnf $(DIRTEST)/par8-1-c.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/par8-1-c.cnf $(DIRTEST)/par8-1-c.sat
	@echo "TEST CASE 16: Parity 8bit"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/par8-1.cnf $(DIRTEST)/par8-1.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/par8-1.cnf $(DIRTEST)/par8-1.sat
	@echo "TEST CASE 17: Parity 16bit"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/par16-1.cnf $(DIRTEST)/par16-1.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/par16-1.cnf $(DIRTEST)/par16-1.sat
	@echo "TEST CASE 18: Parity 16bit (Simplified)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-simple/par16-1-c.cnf $(DIRTEST)/par16-1-c.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-simple/par16-1-c.cnf $(DIRTEST)/par16-1-c.sat
test-m2-hard: all verifier | dir-test
	@echo "###### M2-HARD TEST CASE ######"
	@echo "TEST CASE 3: Parity 32bit"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-hard/par32-1.cnf $(DIRTEST)/par32-1.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-hard/par32-1.cnf $(DIRTEST)/par32-1.sat
	@echo "TEST CASE 4: Parity 32bit (Simplified)"
	$(DIRBIN)/$(EXENAME) benchmarks/m2-hard/par32-1-c.cnf $(DIRTEST)/par32-1-c.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/m2-hard/par32-1-c.cnf $(DIRTEST)/par32-1-c.sat
test-crafted: all verifier | dir-test
	@echo "###### CRAFTED TEST CASE ######"
	@echo "TEST CASE 1: fixed bandwidth"
	$(DIRBIN)/$(EXENAME) benchmarks/crafted/fixed-bandwidth.cnf $(DIRTEST)/fixed-bandwidth.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/crafted/fixed-bandwidth.cnf $(DIRTEST)/fixed-bandwidth.sat
	@echo "TEST CASE 2: ecgrid"
	$(DIRBIN)/$(EXENAME) benchmarks/crafted/ecgrid.cnf $(DIRTEST)/ecgrid.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/crafted/ecgrid.cnf $(DIRTEST)/ecgrid.sat
	@echo "TEST CASE 3: tseiting grid"
	$(DIRBIN)/$(EXENAME) benchmarks/crafted/tseiting-grid.cnf $(DIRTEST)/tseiting-grid.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/crafted/tseiting-grid.cnf $(DIRTEST)/tseiting-grid.sat
	@echo "TEST CASE 4: rphp4"
	$(DIRBIN)/$(EXENAME) benchmarks/crafted/rphp4.cnf $(DIRTEST)/rphp4.sat > /dev/null
	$(BINVERIFIER) -S benchmarks/crafted/rphp4.cnf $(DIRTEST)/rphp4.sat
	@echo "TEST CASE 5: ptn"
	$(DIRBIN)/$(EXENAME) benchmarks/crafted/ptn.cnf $(DIRTEST)/ptn.sat > /dev/null
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
