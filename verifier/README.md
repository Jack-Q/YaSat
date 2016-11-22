YaSat Verifier
==============

YaSat Verifier (`yasat-veri`) is a small utility to verify the result of a SAT Solver.

Usage
-----
```bash
# if the problem is satisfiable, use the `-S' option
yasat-veri -S path/to/SAT-problem.cnf path/to/Solver-output.sat

# if the problem is unsatisfiable, use the `-U' option
yasat-veri -U path/to/SAT-problem.cnf path/to/Solver-output.sat
```
