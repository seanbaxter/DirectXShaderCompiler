LLVM37 Command Guide
------------------

NOTE: the DirectX Compiler project drops support for many of these tools, as
they are no longer suitable or have a more specialized replacement.

The following documents are command descriptions for all of the LLVM37 tools.
These pages describe how to use the LLVM37 commands and what their options are.
Note that these pages do not describe all of the options available for all
tools. To get a complete listing, pass the ``--help`` (general options) or
``--help-hidden`` (general and debugging options) arguments to the tool you are
interested in.

Basic Commands
~~~~~~~~~~~~~~

.. toctree::
   :maxdepth: 1

   llvm37-as
   llvm37-dis
   opt
   llc
   lli
   llvm37-link
   llvm37-ar
   llvm37-nm
   llvm-config
   llvm37-diff
   llvm37-cov
   llvm37-profdata
   llvm37-stress
   llvm37-symbolizer
   llvm37-dwarfdump

Debugging Tools
~~~~~~~~~~~~~~~

.. toctree::
   :maxdepth: 1

   llvm37-extract
   llvm37-bcanalyzer

Developer Tools
~~~~~~~~~~~~~~~

.. toctree::
   :maxdepth: 1

   FileCheck
   tblgen
   lit
   llvm-build
   llvm37-readobj
