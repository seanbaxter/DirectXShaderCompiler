llvm37-extract - extract a function from an LLVM37 module
=====================================================

SYNOPSIS
--------

:program:`llvm37-extract` [*options*] **--func** *function-name* [*filename*]

DESCRIPTION
-----------

The :program:`llvm37-extract` command takes the name of a function and extracts
it from the specified LLVM37 bitcode file.  It is primarily used as a debugging
tool to reduce test cases from larger programs that are triggering a bug.

In addition to extracting the bitcode of the specified function,
:program:`llvm37-extract` will also remove unreachable global variables,
prototypes, and unused types.

The :program:`llvm37-extract` command reads its input from standard input if
filename is omitted or if filename is ``-``.  The output is always written to
standard output, unless the **-o** option is specified (see below).

OPTIONS
-------

**-f**

 Enable binary output on terminals.  Normally, :program:`llvm37-extract` will
 refuse to write raw bitcode output if the output stream is a terminal.  With
 this option, :program:`llvm37-extract` will write raw bitcode regardless of the
 output device.

**--func** *function-name*

 Extract the function named *function-name* from the LLVM37 bitcode.  May be
 specified multiple times to extract multiple functions at once.

**--rfunc** *function-regular-expr*

 Extract the function(s) matching *function-regular-expr* from the LLVM37 bitcode.
 All functions matching the regular expression will be extracted.  May be
 specified multiple times.

**--glob** *global-name*

 Extract the global variable named *global-name* from the LLVM37 bitcode.  May be
 specified multiple times to extract multiple global variables at once.

**--rglob** *glob-regular-expr*

 Extract the global variable(s) matching *global-regular-expr* from the LLVM37
 bitcode.  All global variables matching the regular expression will be
 extracted.  May be specified multiple times.

**-help**

 Print a summary of command line options.

**-o** *filename*

 Specify the output filename.  If filename is "-" (the default), then
 :program:`llvm37-extract` sends its output to standard output.

**-S**

 Write output in LLVM37 intermediate language (instead of bitcode).

EXIT STATUS
-----------

If :program:`llvm37-extract` succeeds, it will exit with 0.  Otherwise, if an error
occurs, it will exit with a non-zero value.

SEE ALSO
--------

bugpoint

