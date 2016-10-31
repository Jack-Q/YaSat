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
SHELL=/bin/bash

EXENAME=yasat

# Debugging flags
FLAGS=  -Wall -Wold-style-cast -Wextra -Wformat=2 \
         -ggdb3 -DDEBUG -std=c++11

# Optimizing flags
# FLAGS=-Wall -Wold-style-cast -Wformat=2 -ansi -pedantic -O3

# List all the .o files you need to build here
HEADER_R:= util.h
HEADERS := yasat.h parser.h \
		   util/exception.h util/io.h
OBJS    := main.o $(HEADERS:.h=.o)
HEADER_R:= $(addprefix $(DIRSRC)/, $(HEADER_R))
HEADERS := $(addprefix $(DIRSRC)/, $(HEADERS)) $(HEADER_R)

.PHONY: clean test dirs tags all

.DEFAULT_GOAL: all

#################### Targets ##################
all: $(DIRBIN)/$(EXENAME) tags

$(DIRBIN)/$(EXENAME): $(addprefix $(DIROBJ)/, $(OBJS))
	$(CXX) $(FLAGS) -o $@ $^

$(DIROBJ)/%.o: $(DIRSRC)/%.cpp $(HEADERS) | dirs
	$(CXX) $(FLAGS) -c -o $@ $< 


##################### PHONY ####################
tags:
	gtags
clean:
	@rm -rf $(addprefix $(DIROBJ)/, $(OBJS)) 
	@rm -rf $(DIRBIN)/$(EXENAME)
	@rm -rf $(DIROBJ)
	@rm -rf GPATH  GRTAGS  GSYMS  GTAGS
test:
	@echo "#### Test ####"
dirs:
	@mkdir -p $(DIROBJ)/{,util}
