#!/bin/sh

# benchmark with plt of the result based on the Dumbbench Perl package
#
# Dependency: Dumbbench, Perl, ROOT,
#

SOLVER="./minisat"
OUTPUT="--stdout>/dev/null"
INPUT="benchmarks"

dumbbench --plot_timings -p 0.001 \
  --code="system(\"${SOLVER} ${INPUT}/sanity/sanity2.cnf     ${OUTPUT}\")" \
  --code="system(\"${SOLVER} ${INPUT}/sanity/sanity3.cnf     ${OUTPUT}\")" \
  --code="system(\"${SOLVER} ${INPUT}/sanity/sanity4.cnf     ${OUTPUT}\")" \
  --code="system(\"${SOLVER} ${INPUT}/sanity/sanity5.cnf     ${OUTPUT}\")" \
  --code="system(\"${SOLVER} ${INPUT}/tiny/rand5_10.cnf      ${OUTPUT}\")" \
  --code="system(\"${SOLVER} ${INPUT}/tiny/rand5_30.cnf      ${OUTPUT}\")" \
  --code="system(\"${SOLVER} ${INPUT}/tiny/rand10_20.cnf     ${OUTPUT}\")" \
  --code="system(\"${SOLVER} ${INPUT}/tiny/rand10_50.cnf     ${OUTPUT}\")" \
  --code="system(\"${SOLVER} ${INPUT}/m2-simple/par8-1.cnf   ${OUTPUT}\")" \
  --code="system(\"${SOLVER} ${INPUT}/m2-simple/par8-1-c.cnf ${OUTPUT}\")" \
  --code="system(\"${SOLVER} ${INPUT}/m2-simple/jnh11.cnf    ${OUTPUT}\")"
