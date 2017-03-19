###############################################
##  YaSat -- Yet Another SAT Solver
##
##  Jack Q (qiaobo@outlook.com)
##  (0540017, CS, NCTU)
###############################################

# Makefile for solver solution verifier

ifeq ("$(CXX)","")
	CXX := g++
endif
SHELL=/bin/bash

EXENAME=yasat-veri

FLAGS=  -Wall -Wold-style-cast -Wextra -Wformat=2 \
         -std=c++14 -O3 -O2 -DDEBUG -DPARSE_EXT_COMPAT

$(EXENAME): main.cpp
	$(CXX) $(FLAGS) -o $@ $<

.PHONY: clean
clean:
	@rm -f $(EXENAME)
