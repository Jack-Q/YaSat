###############################################
##  YaSat -- Yet Another SAT Solver
##
##  Jack Q (qiaobo@outlook.com)
##  (0540017, CS, NCTU)
###############################################

# Makefile for solver solution verifier

CXX=g++
SHELL=/bin/bash

EXENAME=yasat-veri

FLAGS=  -Wall -Wold-style-cast -Wextra -Wformat=2 \
         -std=c++11 -O3 -O2 -DDEBUG

$(EXENAME): main.cpp
	$(CXX) $(FLAGS) -o $@ $<

.PHONY: clean
clean:
	@rm -f $(EXENAME)
