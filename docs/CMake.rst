========================
Building LLVM37 with CMake
========================

.. contents::
   :local:

Introduction
============

NOTE: this document describes the original LLVM37 project, not the DirectX
Compiler. It's available only for informational purposes.

`CMake <http://www.cmake.org/>`_ is a cross-platform build-generator tool. CMake
does not build the project, it generates the files needed by your build tool
(GNU make, Visual Studio, etc) for building LLVM37.

If you are really anxious about getting a functional LLVM37 build, go to the
`Quick start`_ section. If you are a CMake novice, start on `Basic CMake usage`_
and then go back to the `Quick start`_ once you know what you are doing. The
`Options and variables`_ section is a reference for customizing your build. If
you already have experience with CMake, this is the recommended starting point.

.. _Quick start:

Quick start
===========

We use here the command-line, non-interactive CMake interface.

#. `Download <http://www.cmake.org/cmake/resources/software.html>`_ and install
   CMake. Version 2.8.8 is the minimum required.

#. Open a shell. Your development tools must be reachable from this shell
   through the PATH environment variable.

#. Create a directory for containing the build. It is not supported to build
   LLVM37 on the source directory. cd to this directory:

   .. code-block:: console

     $ mkdir mybuilddir
     $ cd mybuilddir

#. Execute this command on the shell replacing `path/to/llvm37/source/root` with
   the path to the root of your LLVM37 source tree:

   .. code-block:: console

     $ cmake path/to/llvm37/source/root

   CMake will detect your development environment, perform a series of test and
   generate the files required for building LLVM37. CMake will use default values
   for all build parameters. See the `Options and variables`_ section for
   fine-tuning your build

   This can fail if CMake can't detect your toolset, or if it thinks that the
   environment is not sane enough. On this case make sure that the toolset that
   you intend to use is the only one reachable from the shell and that the shell
   itself is the correct one for you development environment. CMake will refuse
   to build MinGW makefiles if you have a POSIX shell reachable through the PATH
   environment variable, for instance. You can force CMake to use a given build
   tool, see the `Usage`_ section.

#. After CMake has finished running, proceed to use IDE project files or start
   the build from the build directory:

   .. code-block:: console

     $ cmake --build .

   The ``--build`` option tells ``cmake`` to invoke the underlying build
   tool (``make``, ``ninja``, ``xcodebuild``, ``msbuild``, etc).

   The underlying build tool can be invoked directly either of course, but
   the ``--build`` option is portable.

#. After LLVM37 has finished building, install it from the build directory:

   .. code-block:: console

     $ cmake --build . --target install

   The ``--target`` option with ``install`` parameter in addition to
   the ``--build`` option tells ``cmake`` to build the ``install`` target.

   It is possible to set a different install prefix at installation time
   by invoking the ``cmake_install.cmake`` script generated in the
   build directory:

   .. code-block:: console

     $ cmake -DCMAKE_INSTALL_PREFIX=/tmp/llvm37 -P cmake_install.cmake

.. _Basic CMake usage:
.. _Usage:

Basic CMake usage
=================

This section explains basic aspects of CMake, mostly for explaining those
options which you may need on your day-to-day usage.

CMake comes with extensive documentation in the form of html files and on the
cmake executable itself. Execute ``cmake --help`` for further help options.

CMake requires to know for which build tool it shall generate files (GNU make,
Visual Studio, Xcode, etc). If not specified on the command line, it tries to
guess it based on you environment. Once identified the build tool, CMake uses
the corresponding *Generator* for creating files for your build tool. You can
explicitly specify the generator with the command line option ``-G "Name of the
generator"``. For knowing the available generators on your platform, execute

.. code-block:: console

  $ cmake --help

This will list the generator's names at the end of the help text. Generator's
names are case-sensitive. Example:

.. code-block:: console

  $ cmake -G "Visual Studio 11" path/to/llvm37/source/root

For a given development platform there can be more than one adequate
generator. If you use Visual Studio "NMake Makefiles" is a generator you can use
for building with NMake. By default, CMake chooses the more specific generator
supported by your development environment. If you want an alternative generator,
you must tell this to CMake with the ``-G`` option.

.. todo::

  Explain variables and cache. Move explanation here from #options section.

.. _Options and variables:

Options and variables
=====================

Variables customize how the build will be generated. Options are boolean
variables, with possible values ON/OFF. Options and variables are defined on the
CMake command line like this:

.. code-block:: console

  $ cmake -DVARIABLE=value path/to/llvm37/source

You can set a variable after the initial CMake invocation for changing its
value. You can also undefine a variable:

.. code-block:: console

  $ cmake -UVARIABLE path/to/llvm37/source

Variables are stored on the CMake cache. This is a file named ``CMakeCache.txt``
on the root of the build directory. Do not hand-edit it.

Variables are listed here appending its type after a colon. It is correct to
write the variable and the type on the CMake command line:

.. code-block:: console

  $ cmake -DVARIABLE:TYPE=value path/to/llvm37/source

Frequently-used CMake variables
-------------------------------

Here are some of the CMake variables that are used often, along with a
brief explanation and LLVM37-specific notes. For full documentation, check the
CMake docs or execute ``cmake --help-variable VARIABLE_NAME``.

**CMAKE_BUILD_TYPE**:STRING
  Sets the build type for ``make`` based generators. Possible values are
  Release, Debug, RelWithDebInfo and MinSizeRel. On systems like Visual Studio
  the user sets the build type with the IDE settings.

**CMAKE_INSTALL_PREFIX**:PATH
  Path where LLVM37 will be installed if "make install" is invoked or the
  "INSTALL" target is built.

**LLVM37_LIBDIR_SUFFIX**:STRING
  Extra suffix to append to the directory where libraries are to be
  installed. On a 64-bit architecture, one could use ``-DLLVM37_LIBDIR_SUFFIX=64``
  to install libraries to ``/usr/lib64``.

**CMAKE_C_FLAGS**:STRING
  Extra flags to use when compiling C source files.

**CMAKE_CXX_FLAGS**:STRING
  Extra flags to use when compiling C++ source files.

**BUILD_SHARED_LIBS**:BOOL
  Flag indicating if shared libraries will be built. Its default value is
  OFF. Shared libraries are not supported on Windows and not recommended on the
  other OSes.

.. _LLVM37-specific variables:

LLVM37-specific variables
-----------------------

**LLVM37_TARGETS_TO_BUILD**:STRING
  Semicolon-separated list of targets to build, or *all* for building all
  targets. Case-sensitive. Defaults to *all*. Example:
  ``-DLLVM37_TARGETS_TO_BUILD="X86;PowerPC"``.

**LLVM37_BUILD_TOOLS**:BOOL
  Build LLVM37 tools. Defaults to ON. Targets for building each tool are generated
  in any case. You can build an tool separately by invoking its target. For
  example, you can build *llvm37-as* with a makefile-based system executing *make
  llvm37-as* on the root of your build directory.

**LLVM37_INCLUDE_TOOLS**:BOOL
  Generate build targets for the LLVM37 tools. Defaults to ON. You can use that
  option for disabling the generation of build targets for the LLVM37 tools.

**LLVM37_BUILD_EXAMPLES**:BOOL
  Build LLVM37 examples. Defaults to OFF. Targets for building each example are
  generated in any case. See documentation for *LLVM37_BUILD_TOOLS* above for more
  details.

**LLVM37_INCLUDE_EXAMPLES**:BOOL
  Generate build targets for the LLVM37 examples. Defaults to ON. You can use that
  option for disabling the generation of build targets for the LLVM37 examples.

**LLVM37_BUILD_TESTS**:BOOL
  Build LLVM37 unit tests. Defaults to OFF. Targets for building each unit test
  are generated in any case. You can build a specific unit test with the target
  *UnitTestNameTests* (where at this time *UnitTestName* can be ADT, Analysis,
  ExecutionEngine, JIT, Support, Transform, VMCore; see the subdirectories of
  *unittests* for an updated list.) It is possible to build all unit tests with
  the target *UnitTests*.

**LLVM37_INCLUDE_TESTS**:BOOL
  Generate build targets for the LLVM37 unit tests. Defaults to ON. You can use
  that option for disabling the generation of build targets for the LLVM37 unit
  tests.

**LLVM37_APPEND_VC_REV**:BOOL
  Append version control revision info (svn revision number or Git revision id)
  to LLVM37 version string (stored in the PACKAGE_VERSION macro). For this to work
  cmake must be invoked before the build. Defaults to OFF.

**LLVM37_ENABLE_THREADS**:BOOL
  Build with threads support, if available. Defaults to ON.

**LLVM37_ENABLE_CXX1Y**:BOOL
  Build in C++1y mode, if available. Defaults to OFF.

**LLVM37_ENABLE_ASSERTIONS**:BOOL
  Enables code assertions. Defaults to ON if and only if ``CMAKE_BUILD_TYPE``
  is *Debug*.

**LLVM37_ENABLE_EH**:BOOL
  Build LLVM37 with exception handling support. This is necessary if you wish to
  link against LLVM37 libraries and make use of C++ exceptions in your own code
  that need to propagate through LLVM37 code. Defaults to OFF.

**LLVM37_ENABLE_PIC**:BOOL
  Add the ``-fPIC`` flag for the compiler command-line, if the compiler supports
  this flag. Some systems, like Windows, do not need this flag. Defaults to ON.

**LLVM37_ENABLE_RTTI**:BOOL
  Build LLVM37 with run time type information. Defaults to OFF.

**LLVM37_ENABLE_WARNINGS**:BOOL
  Enable all compiler warnings. Defaults to ON.

**LLVM37_ENABLE_PEDANTIC**:BOOL
  Enable pedantic mode. This disables compiler specific extensions, if
  possible. Defaults to ON.

**LLVM37_ENABLE_WERROR**:BOOL
  Stop and fail build, if a compiler warning is triggered. Defaults to OFF.

**LLVM37_ABI_BREAKING_CHECKS**:STRING
  Used to decide if LLVM37 should be built with ABI breaking checks or
  not.  Allowed values are `WITH_ASSERTS` (default), `FORCE_ON` and
  `FORCE_OFF`.  `WITH_ASSERTS` turns on ABI breaking checks in an
  assertion enabled build.  `FORCE_ON` (`FORCE_OFF`) turns them on
  (off) irrespective of whether normal (`NDEBUG` based) assertions are
  enabled or not.  A version of LLVM37 built with ABI breaking checks
  is not ABI compatible with a version built without it.

**LLVM37_BUILD_32_BITS**:BOOL
  Build 32-bits executables and libraries on 64-bits systems. This option is
  available only on some 64-bits unix systems. Defaults to OFF.

**LLVM37_TARGET_ARCH**:STRING
  LLVM37 target to use for native code generation. This is required for JIT
  generation. It defaults to "host", meaning that it shall pick the architecture
  of the machine where LLVM37 is being built. If you are cross-compiling, set it
  to the target architecture name.

**LLVM37_TABLEGEN**:STRING
  Full path to a native TableGen executable (usually named ``tblgen``). This is
  intended for cross-compiling: if the user sets this variable, no native
  TableGen will be created.

**LLVM37_LIT_ARGS**:STRING
  Arguments given to lit.  ``make check`` and ``make clang-test`` are affected.
  By default, ``'-sv --no-progress-bar'`` on Visual C++ and Xcode, ``'-sv'`` on
  others.

**LLVM37_LIT_TOOLS_DIR**:PATH
  The path to GnuWin32 tools for tests. Valid on Windows host.  Defaults to "",
  then Lit seeks tools according to %PATH%.  Lit can find tools(eg. grep, sort,
  &c) on LLVM37_LIT_TOOLS_DIR at first, without specifying GnuWin32 to %PATH%.

**LLVM37_ENABLE_FFI**:BOOL
  Indicates whether LLVM37 Interpreter will be linked with Foreign Function
  Interface library. If the library or its headers are installed on a custom
  location, you can set the variables FFI_INCLUDE_DIR and
  FFI_LIBRARY_DIR. Defaults to OFF.

**LLVM37_EXTERNAL_{CLANG,LLD,POLLY}_SOURCE_DIR**:PATH
  Path to ``{Clang,lld,Polly}``\'s source directory. Defaults to
  ``tools/{clang,lld,polly}``. ``{Clang,lld,Polly}`` will not be built when it
  is empty or it does not point to a valid path.

**LLVM37_USE_OPROFILE**:BOOL
  Enable building OProfile JIT support. Defaults to OFF

**LLVM37_USE_INTEL_JITEVENTS**:BOOL
  Enable building support for Intel JIT Events API. Defaults to OFF

**LLVM37_ENABLE_ZLIB**:BOOL
  Build with zlib to support compression/uncompression in LLVM37 tools.
  Defaults to ON.

**LLVM37_USE_SANITIZER**:STRING
  Define the sanitizer used to build LLVM37 binaries and tests. Possible values
  are ``Address``, ``Memory``, ``MemoryWithOrigins``, ``Undefined``, ``Thread``,
  and ``Address;Undefined``. Defaults to empty string.

**LLVM37_PARALLEL_COMPILE_JOBS**:STRING
  Define the maximum number of concurrent compilation jobs.

**LLVM37_PARALLEL_LINK_JOBS**:STRING
  Define the maximum number of concurrent link jobs.

**LLVM37_BUILD_DOCS**:BOOL
  Enables all enabled documentation targets (i.e. Doxgyen and Sphinx targets) to
  be built as part of the normal build. If the ``install`` target is run then
  this also enables all built documentation targets to be installed. Defaults to
  OFF.

**LLVM37_ENABLE_DOXYGEN**:BOOL
  Enables the generation of browsable HTML documentation using doxygen.
  Defaults to OFF.

**LLVM37_ENABLE_DOXYGEN_QT_HELP**:BOOL
  Enables the generation of a Qt Compressed Help file. Defaults to OFF.
  This affects the make target ``doxygen-llvm37``. When enabled, apart from
  the normal HTML output generated by doxygen, this will produce a QCH file
  named ``org.llvm37.qch``. You can then load this file into Qt Creator.
  This option is only useful in combination with ``-DLLVM37_ENABLE_DOXYGEN=ON``;
  otherwise this has no effect.

**LLVM37_DOXYGEN_QCH_FILENAME**:STRING
  The filename of the Qt Compressed Help file that will be generated when
  ``-DLLVM37_ENABLE_DOXYGEN=ON`` and
  ``-DLLVM37_ENABLE_DOXYGEN_QT_HELP=ON`` are given. Defaults to
  ``org.llvm37.qch``.
  This option is only useful in combination with
  ``-DLLVM37_ENABLE_DOXYGEN_QT_HELP=ON``;
  otherwise this has no effect.

**LLVM37_DOXYGEN_QHP_NAMESPACE**:STRING
  Namespace under which the intermediate Qt Help Project file lives. See `Qt
  Help Project`_
  for more information. Defaults to "org.llvm37". This option is only useful in
  combination with ``-DLLVM37_ENABLE_DOXYGEN_QT_HELP=ON``; otherwise
  this has no effect.

**LLVM37_DOXYGEN_QHP_CUST_FILTER_NAME**:STRING
  See `Qt Help Project`_ for
  more information. Defaults to the CMake variable ``${PACKAGE_STRING}`` which
  is a combination of the package name and version string. This filter can then
  be used in Qt Creator to select only documentation from LLVM37 when browsing
  through all the help files that you might have loaded. This option is only
  useful in combination with ``-DLLVM37_ENABLE_DOXYGEN_QT_HELP=ON``;
  otherwise this has no effect.

.. _Qt Help Project: http://qt-project.org/doc/qt-4.8/qthelpproject.html#custom-filters

**LLVM37_DOXYGEN_QHELPGENERATOR_PATH**:STRING
  The path to the ``qhelpgenerator`` executable. Defaults to whatever CMake's
  ``find_program()`` can find. This option is only useful in combination with
  ``-DLLVM37_ENABLE_DOXYGEN_QT_HELP=ON``; otherwise this has no
  effect.

**LLVM37_DOXYGEN_SVG**:BOOL
  Uses .svg files instead of .png files for graphs in the Doxygen output.
  Defaults to OFF.

**LLVM37_ENABLE_SPHINX**:BOOL
  If enabled CMake will search for the ``sphinx-build`` executable and will make
  the ``SPHINX_OUTPUT_HTML`` and ``SPHINX_OUTPUT_MAN`` CMake options available.
  Defaults to OFF.

**SPHINX_EXECUTABLE**:STRING
  The path to the ``sphinx-build`` executable detected by CMake.

**SPHINX_OUTPUT_HTML**:BOOL
  If enabled (and ``LLVM37_ENABLE_SPHINX`` is enabled) then the targets for
  building the documentation as html are added (but not built by default unless
  ``LLVM37_BUILD_DOCS`` is enabled). There is a target for each project in the
  source tree that uses sphinx (e.g.  ``docs-llvm37-html``, ``docs-clang-html``
  and ``docs-lld-html``). Defaults to ON.

**SPHINX_OUTPUT_MAN**:BOOL
  If enabled (and ``LLVM37_ENABLE_SPHINX`` is enabled) the targets for building
  the man pages are added (but not built by default unless ``LLVM37_BUILD_DOCS``
  is enabled). Currently the only target added is ``docs-llvm37-man``. Defaults
  to ON.

**SPHINX_WARNINGS_AS_ERRORS**:BOOL
  If enabled then sphinx documentation warnings will be treated as
  errors. Defaults to ON.

Executing the test suite
========================

Testing is performed when the *check* target is built. For instance, if you are
using makefiles, execute this command while on the top level of your build
directory:

.. code-block:: console

  $ make check

On Visual Studio, you may run tests to build the project "check".

Cross compiling
===============

See `this wiki page <http://www.vtk.org/Wiki/CMake_Cross_Compiling>`_ for
generic instructions on how to cross-compile with CMake. It goes into detailed
explanations and may seem daunting, but it is not. On the wiki page there are
several examples including toolchain files. Go directly to `this section
<http://www.vtk.org/Wiki/CMake_Cross_Compiling#Information_how_to_set_up_various_cross_compiling_toolchains>`_
for a quick solution.

Also see the `LLVM37-specific variables`_ section for variables used when
cross-compiling.

Embedding LLVM37 in your project
==============================

From LLVM37 3.5 onwards both the CMake and autoconf/Makefile build systems export
LLVM37 libraries as importable CMake targets. This means that clients of LLVM37 can
now reliably use CMake to develop their own LLVM37 based projects against an
installed version of LLVM37 regardless of how it was built.

Here is a simple example of CMakeLists.txt file that imports the LLVM37 libraries
and uses them to build a simple application ``simple-tool``.

.. code-block:: cmake

  cmake_minimum_required(VERSION 2.8.8)
  project(SimpleProject)

  find_package(LLVM37 REQUIRED CONFIG)

  message(STATUS "Found LLVM37 ${LLVM37_PACKAGE_VERSION}")
  message(STATUS "Using LLVMConfig.cmake in: ${LLVM37_DIR}")

  # Set your project compile flags.
  # E.g. if using the C++ header files
  # you will need to enable C++11 support
  # for your compiler.

  include_directories(${LLVM37_INCLUDE_DIRS})
  add_definitions(${LLVM37_DEFINITIONS})

  # Now build our tools
  add_executable(simple-tool tool.cpp)

  # Find the libraries that correspond to the LLVM37 components
  # that we wish to use
  llvm37_map_components_to_libnames(llvm37_libs support core irreader)

  # Link against LLVM37 libraries
  target_link_libraries(simple-tool ${llvm37_libs})

The ``find_package(...)`` directive when used in CONFIG mode (as in the above
example) will look for the ``LLVMConfig.cmake`` file in various locations (see
cmake manual for details).  It creates a ``LLVM37_DIR`` cache entry to save the
directory where ``LLVMConfig.cmake`` is found or allows the user to specify the
directory (e.g. by passing ``-DLLVM37_DIR=/usr/share/llvm37/cmake`` to
the ``cmake`` command or by setting it directly in ``ccmake`` or ``cmake-gui``).

This file is available in two different locations.

* ``<INSTALL_PREFIX>/share/llvm37/cmake/LLVMConfig.cmake`` where
  ``<INSTALL_PREFIX>`` is the install prefix of an installed version of LLVM37.
  On Linux typically this is ``/usr/share/llvm37/cmake/LLVMConfig.cmake``.

* ``<LLVM37_BUILD_ROOT>/share/llvm37/cmake/LLVMConfig.cmake`` where
  ``<LLVM37_BUILD_ROOT>`` is the root of the LLVM37 build tree. **Note this only
  available when building LLVM37 with CMake**

If LLVM37 is installed in your operating system's normal installation prefix (e.g.
on Linux this is usually ``/usr/``) ``find_package(LLVM37 ...)`` will
automatically find LLVM37 if it is installed correctly. If LLVM37 is not installed
or you wish to build directly against the LLVM37 build tree you can use
``LLVM37_DIR`` as previously mentioned.

The ``LLVMConfig.cmake`` file sets various useful variables. Notable variables
include

``LLVM37_CMAKE_DIR``
  The path to the LLVM37 CMake directory (i.e. the directory containing
  LLVMConfig.cmake).

``LLVM37_DEFINITIONS``
  A list of preprocessor defines that should be used when building against LLVM37.

``LLVM37_ENABLE_ASSERTIONS``
  This is set to ON if LLVM37 was built with assertions, otherwise OFF.

``LLVM37_ENABLE_EH``
  This is set to ON if LLVM37 was built with exception handling (EH) enabled,
  otherwise OFF.

``LLVM37_ENABLE_RTTI``
  This is set to ON if LLVM37 was built with run time type information (RTTI),
  otherwise OFF.

``LLVM37_INCLUDE_DIRS``
  A list of include paths to directories containing LLVM37 header files.

``LLVM37_PACKAGE_VERSION``
  The LLVM37 version. This string can be used with CMake conditionals. E.g. ``if
  (${LLVM37_PACKAGE_VERSION} VERSION_LESS "3.5")``.

``LLVM37_TOOLS_BINARY_DIR``
  The path to the directory containing the LLVM37 tools (e.g. ``llvm37-as``).

Notice that in the above example we link ``simple-tool`` against several LLVM37
libraries. The list of libraries is determined by using the
``llvm37_map_components_to_libnames()`` CMake function. For a list of available
components look at the output of running ``llvm-config --components``.

Note that for LLVM37 < 3.5 ``llvm37_map_components_to_libraries()`` was
used instead of ``llvm37_map_components_to_libnames()``. This is now deprecated
and will be removed in a future version of LLVM37.

.. _cmake-out-of-source-pass:

Developing LLVM37 passes out of source
------------------------------------

It is possible to develop LLVM37 passes out of LLVM37's source tree (i.e. against an
installed or built LLVM37). An example of a project layout is provided below.

.. code-block:: none

  <project dir>/
      |
      CMakeLists.txt
      <pass name>/
          |
          CMakeLists.txt
          Pass.cpp
          ...

Contents of ``<project dir>/CMakeLists.txt``:

.. code-block:: cmake

  find_package(LLVM37 REQUIRED CONFIG)

  add_definitions(${LLVM37_DEFINITIONS})
  include_directories(${LLVM37_INCLUDE_DIRS})

  add_subdirectory(<pass name>)

Contents of ``<project dir>/<pass name>/CMakeLists.txt``:

.. code-block:: cmake

  add_library(LLVM37Passname MODULE Pass.cpp)

Note if you intend for this pass to be merged into the LLVM37 source tree at some
point in the future it might make more sense to use LLVM37's internal
add_llvm37_loadable_module function instead by...


Adding the following to ``<project dir>/CMakeLists.txt`` (after
``find_package(LLVM37 ...)``)

.. code-block:: cmake

  list(APPEND CMAKE_MODULE_PATH "${LLVM37_CMAKE_DIR}")
  include(AddLLVM)

And then changing ``<project dir>/<pass name>/CMakeLists.txt`` to

.. code-block:: cmake

  add_llvm37_loadable_module(LLVM37Passname
    Pass.cpp
    )

When you are done developing your pass, you may wish to integrate it
into LLVM37 source tree. You can achieve it in two easy steps:

#. Copying ``<pass name>`` folder into ``<LLVM37 root>/lib/Transform`` directory.

#. Adding ``add_subdirectory(<pass name>)`` line into
   ``<LLVM37 root>/lib/Transform/CMakeLists.txt``.

Compiler/Platform-specific topics
=================================

Notes for specific compilers and/or platforms.

Microsoft Visual C++
--------------------

**LLVM37_COMPILER_JOBS**:STRING
  Specifies the maximum number of parallell compiler jobs to use per project
  when building with msbuild or Visual Studio. Only supported for the Visual
  Studio 2010 CMake generator. 0 means use all processors. Default is 0.
