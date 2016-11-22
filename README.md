
YaSat -- Yet Another SAT solver
===============================

> YaSat is yet another SAT solver.

A SAT solver is a program that try to solve the satisfiability problem,
which is one of the first proved NP-complete problem.
The concept of satisfiability problem is quite simple. Just try to find
an assignment to a set of boolean variable whit which a boolean expression
can be deduced to boolean value true (this is satisfiable) or stating that
the boolean expression is impossible to be deduced to boolean true with
any given set of assignment (this is unsatisfiable).
Generally, the SAT solver are try to solve the boolean expression in
CNF (Conjunctive Normal Form), but it can be proved that any boolean can
be converted to such form.

Since most of the decision or design can be divided into a limited number
of boolean decision, under certain constraints, a well implemented SAT solver
can tackle a larger set of practical problem. Currently, SAT solver is the
core component of EDA as well as lots of other applications.

This Implementation
-------------------

Currently, this implementation is merely a naive one with limited capability to
solve problems with less than 20 variables and 50 clauses.

Road map
-------

The procedure of this implementation is conforming the following stages:

1. Naive implementation constructing solver framework
1. Novice implementation with heuristic branching (simple)
1. Boolean constraints propagation feature
1. 2-literal watching
1. Conflict driven clause learning
1. Non-chronological backtracking
1. Effective preprocessing
1. Random restart
1. Parallel


Resource
--------
* [CNFgen on GitHub](https://github.com/MassimoLauria/cnfgen):
  a python toolkit to generate CNF by encoding some kinds of NP problem with specific parameter
* [SAT Competition benchmarks](http://baldur.iti.kit.edu/sat-competition-2016/index.php?cat=downloads)
  benchmark for the SAT Competition
