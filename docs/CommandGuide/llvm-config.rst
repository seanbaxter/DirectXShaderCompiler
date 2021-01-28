llvm-config - Print LLVM37 compilation options
============================================


SYNOPSIS
--------


**llvm-config** *option* [*components*...]


DESCRIPTION
-----------


**llvm-config** makes it easier to build applications that use LLVM37.  It can
print the compiler flags, linker flags and object libraries needed to link
against LLVM37.


EXAMPLES
--------


To link against the JIT:


.. code-block:: sh

   g++ `llvm-config --cxxflags` -o HowToUseJIT.o -c HowToUseJIT.cpp
   g++ `llvm-config --ldflags` -o HowToUseJIT HowToUseJIT.o \
       `llvm-config --libs engine bcreader scalaropts`



OPTIONS
-------



**--version**

 Print the version number of LLVM37.



**-help**

 Print a summary of **llvm-config** arguments.



**--prefix**

 Print the installation prefix for LLVM37.



**--src-root**

 Print the source root from which LLVM37 was built.



**--obj-root**

 Print the object root used to build LLVM37.



**--bindir**

 Print the installation directory for LLVM37 binaries.



**--includedir**

 Print the installation directory for LLVM37 headers.



**--libdir**

 Print the installation directory for LLVM37 libraries.



**--cxxflags**

 Print the C++ compiler flags needed to use LLVM37 headers.



**--ldflags**

 Print the flags needed to link against LLVM37 libraries.



**--libs**

 Print all the libraries needed to link against the specified LLVM37
 *components*, including any dependencies.



**--libnames**

 Similar to **--libs**, but prints the bare filenames of the libraries
 without **-l** or pathnames.  Useful for linking against a not-yet-installed
 copy of LLVM37.



**--libfiles**

 Similar to **--libs**, but print the full path to each library file.  This is
 useful when creating makefile dependencies, to ensure that a tool is relinked if
 any library it uses changes.



**--components**

 Print all valid component names.



**--targets-built**

 Print the component names for all targets supported by this copy of LLVM37.



**--build-mode**

 Print the build mode used when LLVM37 was built (e.g. Debug or Release)




COMPONENTS
----------


To print a list of all available components, run **llvm-config
--components**.  In most cases, components correspond directly to LLVM37
libraries.  Useful "virtual" components include:


**all**

 Includes all LLVM37 libraries.  The default if no components are specified.



**backend**

 Includes either a native backend or the C backend.



**engine**

 Includes either a native JIT or the bitcode interpreter.




EXIT STATUS
-----------


If **llvm-config** succeeds, it will exit with 0.  Otherwise, if an error
occurs, it will exit with a non-zero value.
