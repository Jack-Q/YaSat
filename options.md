# Command Options

## Summary

The options of this program consists of two part: the compiling time
options via parameters of `make`, and the execution time options via
parameters of `yasat` executable. Some features can be configured in
both methods, which is out of the considerations to the improvement
the execution time performance. Most of the compiling time options
are related to debugging usage while most of the execution time options
are related to feature tweaks as well as performance statistics.

## Compiling Options

Compiling options are following the conversion that setting a flag
to any non-blank value toggles is on, unless explicit notes.

-   `FLAGS_DEBUG`: toggle the debug mode. When enabled, the debug
    informations will be embedded into the binary (via `-ggdb3` option)
    and will not enable higher level compiler optimization
    (via`-O2`, `-O3`). Disabled by default.
-   `FLAGS_COLOR`: toggle the color output. When enabled,
    the output message will be formatted with color in supported terminals.
    However, when redirect the message to file for later check,
    the ANSI control sequence will be seen. In this scenario,
    disable the option or read output file via `cat` command.
    Enabled by default.
-   `FLAGS_VERBOSE`: toggle the verbose mode. When enabled, the program
    will print detailed information about the execution procedure. This can
    dramatically decrease the performance of the problem and transform it
    from CPU bounded application to IO bounded application. This is useful
    to find out the source of error during debugging. Disabled by default.
-   `FLAGS_PRINT_STATIS`: toggle the print of statistic data. When enabled,
    the program will print some statistic data of execution state every 3000
    descstion. Currently, the program will print the number of decision,
     the number of implication, the number of conflict, the number of current
     learnt clause, and the maxium level of backtracking.
-   `FLAGS_LITERAL_WEIGHT_DECAY`: toggle the literal weight decaying mode.
    When enabled, the weight of literal will decay as the excution of the
    solver. Current decaying strantegy is multiple $\frac{3}{4}$ every 200
    decision.
    This option is independent with the following weight update option.
-   `FLAGS_LITERAL_WEIGHT_UPDATE`: toggle the literal weight update mode.
    When enabled, the weight of literal will be updated when new clause
    (the learnt conflict clause) is add into database.

## Execution Options

Execution options are following the conversion that any option should
begin with single dash or double dash, that is the former is abbreviation
of some common case of the latter. Apart from the options, any configuration
value should start without dash. Most of the options should have
default value apart form the input and output file that ought to be
specified explicitly. In summary, the command line option for executable
can be used as following pattern:

```bash
./yasat [options] <input file | --stdin> <output file | --stdout >
```
Required options:
-   **`--stdin` or input file:**
    the input source which should be a stream of ascii characters
    representing a SAT problem in CNF format
-   **`--stdout` or output file:**
    the output file of the result.

Optional options:
-   *N/A*
