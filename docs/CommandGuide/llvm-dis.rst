llvm37-dis - LLVM37 disassembler
============================


SYNOPSIS
--------


**llvm37-dis** [*options*] [*filename*]


DESCRIPTION
-----------


The **llvm37-dis** command is the LLVM37 disassembler.  It takes an LLVM37
bitcode file and converts it into human-readable LLVM37 assembly language.

If filename is omitted or specified as ``-``, **llvm37-dis** reads its
input from standard input.

If the input is being read from standard input, then **llvm37-dis**
will send its output to standard output by default.  Otherwise, the
output will be written to a file named after the input file, with
a ``.ll`` suffix added (any existing ``.bc`` suffix will first be
removed).  You can override the choice of output file using the
**-o** option.


OPTIONS
-------



**-f**

 Enable binary output on terminals.  Normally, **llvm37-dis** will refuse to
 write raw bitcode output if the output stream is a terminal. With this option,
 **llvm37-dis** will write raw bitcode regardless of the output device.



**-help**

 Print a summary of command line options.



**-o** *filename*

 Specify the output file name.  If *filename* is -, then the output is sent
 to standard output.




EXIT STATUS
-----------


If **llvm37-dis** succeeds, it will exit with 0.  Otherwise, if an error
occurs, it will exit with a non-zero value.


SEE ALSO
--------


llvm37-as|llvm37-as
