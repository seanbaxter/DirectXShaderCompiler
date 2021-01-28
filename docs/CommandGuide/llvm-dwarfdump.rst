llvm37-dwarfdump - print contents of DWARF sections
=================================================

SYNOPSIS
--------

:program:`llvm37-dwarfdump` [*options*] [*filenames...*]

DESCRIPTION
-----------

:program:`llvm37-dwarfdump` parses DWARF sections in the object files
and prints their contents in human-readable form.

OPTIONS
-------

.. option:: -debug-dump=section

  Specify the DWARF section to dump.
  For example, use ``abbrev`` to dump the contents of ``.debug_abbrev`` section,
  ``loc.dwo`` to dump the contents of ``.debug_loc.dwo`` etc.
  See ``llvm37-dwarfdump --help`` for the complete list of supported sections.
  Use ``all`` to dump all DWARF sections. It is the default.

EXIT STATUS
-----------

:program:`llvm37-dwarfdump` returns 0 if the input files were parsed and dumped
successfully. Otherwise, it returns 1.
